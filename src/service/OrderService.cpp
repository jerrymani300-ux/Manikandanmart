#include "service/OrderService.hpp"
#include "repository/DatabaseConnection.hpp"
#include "exception/AppException.hpp"
#include <spdlog/spdlog.h>

namespace manimart::service {

OrderService::OrderService(
    std::shared_ptr<repository::IOrderRepository> order_repo,
    std::shared_ptr<repository::ICartRepository> cart_repo,
    std::shared_ptr<repository::IProductRepository> product_repo,
    std::shared_ptr<repository::IUserRepository> user_repo,
    std::shared_ptr<provider::IPaymentStrategy> payment_strategy)
    : order_repo_(std::move(order_repo)),
      cart_repo_(std::move(cart_repo)),
      product_repo_(std::move(product_repo)),
      user_repo_(std::move(user_repo)),
      payment_strategy_(std::move(payment_strategy)) {
    if (!order_repo_ || !cart_repo_ || !product_repo_ || !user_repo_ || !payment_strategy_) {
        throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "OrderService: dependencies cannot be null");
    }
}

dto::OrderResponse OrderService::Checkout(int64_t buyer_id, const dto::CheckoutRequest& req) {
    // 1. Verify Buyer exists
    auto buyer = user_repo_->FindById(buyer_id);
    if (!buyer.has_value()) {
        throw exception::NotFoundException("Buyer user account not found", "buyer_id");
    }

    // 2. Fetch and verify cart
    auto cart_items = cart_repo_->FindByUserId(buyer_id);
    if (cart_items.empty()) {
        throw exception::ValidationException("Your cart is empty. Add products before checking out.", "cart");
    }

    // 3. Verify stock and calculate exact total from DB product records
    util::Money total_amount(0);
    std::vector<model::OrderItem> order_items;
    order_items.reserve(cart_items.size());

    for (const auto& cart_item : cart_items) {
        auto prod = product_repo_->FindById(cart_item.product_id);
        if (!prod.has_value()) {
            throw exception::NotFoundException("Product '" + cart_item.product_name + "' no longer exists", "product_id");
        }

        if (prod->stock_qty < cart_item.quantity) {
            throw exception::ValidationException(
                "Insufficient stock for product '" + prod->name + "'. Available: " + std::to_string(prod->stock_qty) + ", requested: " + std::to_string(cart_item.quantity),
                "stock"
            );
        }

        util::Money item_subtotal = prod->price * cart_item.quantity;
        total_amount += item_subtotal;

        model::OrderItem oi{
            .product_id = prod->id,
            .product_name = prod->name,
            .product_image = prod->image_url,
            .quantity = cart_item.quantity,
            .unit_price = prod->price
        };
        order_items.push_back(oi);
    }

    // 4. Process mock payment via Strategy
    auto payment_res = payment_strategy_->ProcessPayment(
        buyer_id, total_amount, req.payment_method.empty() ? "MOCK_CARD" : req.payment_method, req.shipping_address
    );

    if (!payment_res.success) {
        throw exception::ValidationException("Payment failed: " + payment_res.message, "payment");
    }

    // 5. Execute checkout in an atomic database transaction
    int64_t order_id = repository::DatabaseConnection::GetInstance().InTransaction<int64_t>([&]() -> int64_t {
        model::Order new_order{
            .buyer_id = buyer_id,
            .buyer_name = buyer->name,
            .buyer_email = buyer->email,
            .status = model::OrderStatus::CONFIRMED,
            .total_amount = total_amount,
            .items = order_items
        };

        int64_t id = order_repo_->CreateOrder(new_order);
        if (id <= 0) {
            throw exception::DatabaseException("Failed to persist order in database");
        }

        // Decrement stock for all items
        for (const auto& item : order_items) {
            if (!product_repo_->UpdateStock(item.product_id, -item.quantity)) {
                throw exception::DatabaseException("Failed to decrement inventory for product ID " + std::to_string(item.product_id));
            }
        }

        // Clear user's cart
        cart_repo_->ClearCart(buyer_id);

        return id;
    });

    spdlog::info("Order #{} placed successfully by buyer {} for total {}", order_id, buyer_id, total_amount.ToFormattedString());

    auto created_order = order_repo_->FindById(order_id);
    if (!created_order.has_value()) {
        throw exception::DatabaseException("Failed to retrieve created order");
    }

    return dto::OrderDtoBuilder().FromModel(*created_order).Build();
}

dto::OrderResponse OrderService::GetOrderById(int64_t user_id, int64_t order_id, bool is_admin) {
    auto order_opt = order_repo_->FindById(order_id);
    if (!order_opt.has_value()) {
        throw exception::NotFoundException("Order not found", "id");
    }

    // Permission check: Buyer owns it, or Admin
    if (!is_admin && order_opt->buyer_id != user_id) {
        throw exception::AuthorizationException("You are not authorized to view this order");
    }

    return dto::OrderDtoBuilder().FromModel(*order_opt).Build();
}

std::vector<dto::OrderResponse> OrderService::GetBuyerOrders(int64_t buyer_id) {
    auto orders = order_repo_->FindByBuyerId(buyer_id);
    std::vector<dto::OrderResponse> dtos;
    dtos.reserve(orders.size());
    for (const auto& ord : orders) {
        dtos.push_back(dto::OrderDtoBuilder().FromModel(ord).Build());
    }
    return dtos;
}

std::vector<dto::OrderResponse> OrderService::GetSellerOrders(int64_t seller_id) {
    auto orders = order_repo_->FindBySellerId(seller_id);
    std::vector<dto::OrderResponse> dtos;
    dtos.reserve(orders.size());
    for (const auto& ord : orders) {
        dtos.push_back(dto::OrderDtoBuilder().FromModel(ord).Build());
    }
    return dtos;
}

std::vector<dto::OrderResponse> OrderService::GetAllOrders(int limit, int offset) {
    auto orders = order_repo_->FindAll(limit, offset);
    std::vector<dto::OrderResponse> dtos;
    dtos.reserve(orders.size());
    for (const auto& ord : orders) {
        dtos.push_back(dto::OrderDtoBuilder().FromModel(ord).Build());
    }
    return dtos;
}

dto::OrderResponse OrderService::UpdateOrderStatus(int64_t /*user_id*/, int64_t order_id, const std::string& new_status_str, bool /*is_admin*/) {
    auto order_opt = order_repo_->FindById(order_id);
    if (!order_opt.has_value()) {
        throw exception::NotFoundException("Order not found", "order_id");
    }

    model::OrderStatus new_status = model::StringToOrderStatus(new_status_str);
    if (!model::CanTransition(order_opt->status, new_status)) {
        throw exception::ValidationException(
            "Cannot transition order status from " + model::OrderStatusToString(order_opt->status) + " to " + new_status_str,
            "status"
        );
    }

    if (!order_repo_->UpdateStatus(order_id, new_status)) {
        throw exception::DatabaseException("Failed to update order status");
    }

    auto updated = order_repo_->FindById(order_id);
    return dto::OrderDtoBuilder().FromModel(*updated).Build();
}

dto::OrderResponse OrderService::CancelOrder(int64_t buyer_id, int64_t order_id) {
    auto order_opt = order_repo_->FindById(order_id);
    if (!order_opt.has_value()) {
        throw exception::NotFoundException("Order not found", "order_id");
    }

    if (order_opt->buyer_id != buyer_id) {
        throw exception::AuthorizationException("You can only cancel your own orders");
    }

    if (order_opt->status != model::OrderStatus::PENDING && order_opt->status != model::OrderStatus::CONFIRMED) {
        throw exception::ValidationException("Order cannot be cancelled in its current state (" + model::OrderStatusToString(order_opt->status) + ")", "status");
    }

    // Atomic cancellation: Update status and restore product stock
    repository::DatabaseConnection::GetInstance().InTransaction<bool>([&]() -> bool {
        order_repo_->UpdateStatus(order_id, model::OrderStatus::CANCELLED);
        for (const auto& item : order_opt->items) {
            product_repo_->UpdateStock(item.product_id, item.quantity);
        }
        return true;
    });

    auto updated = order_repo_->FindById(order_id);
    return dto::OrderDtoBuilder().FromModel(*updated).Build();
}

} // namespace manimart::service
