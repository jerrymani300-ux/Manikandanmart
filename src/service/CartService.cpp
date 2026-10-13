#include "service/CartService.hpp"
#include "exception/AppException.hpp"
#include <spdlog/spdlog.h>

namespace manimart::service {

CartService::CartService(
    std::shared_ptr<repository::ICartRepository> cart_repo,
    std::shared_ptr<repository::IProductRepository> product_repo)
    : cart_repo_(std::move(cart_repo)), product_repo_(std::move(product_repo)) {
    if (!cart_repo_ || !product_repo_) {
        throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "CartService: repositories cannot be null");
    }
}

dto::CartResponse CartService::BuildCartResponse(int64_t user_id) {
    auto items = cart_repo_->FindByUserId(user_id);
    dto::CartResponse response;
    util::Money grand_total(0);
    int total_item_count = 0;

    for (const auto& item : items) {
        util::Money subtotal = item.product_price * item.quantity;
        grand_total += subtotal;
        total_item_count += item.quantity;

        dto::CartItemResponse item_dto{
            .id = item.id,
            .product_id = item.product_id,
            .product_name = item.product_name,
            .product_category = item.product_category,
            .product_image_url = item.product_image_url,
            .unit_price_cents = item.product_price.GetCents(),
            .formatted_unit_price = item.product_price.ToFormattedString(),
            .product_stock = item.product_stock,
            .quantity = item.quantity,
            .subtotal_cents = subtotal.GetCents(),
            .formatted_subtotal = subtotal.ToFormattedString()
        };
        response.items.push_back(item_dto);
    }

    response.total_items = total_item_count;
    response.total_amount_cents = grand_total.GetCents();
    response.formatted_total = grand_total.ToFormattedString();
    return response;
}

dto::CartResponse CartService::GetUserCart(int64_t user_id) {
    return BuildCartResponse(user_id);
}

dto::CartResponse CartService::AddToCart(int64_t user_id, const dto::AddToCartRequest& req) {
    if (req.quantity <= 0) {
        throw exception::ValidationException("Quantity must be greater than 0", "quantity");
    }

    auto product_opt = product_repo_->FindById(req.product_id);
    if (!product_opt.has_value()) {
        throw exception::NotFoundException("Product not found", "product_id");
    }

    auto existing_item = cart_repo_->FindByUserAndProduct(user_id, req.product_id);
    int requested_total_qty = req.quantity + (existing_item ? existing_item->quantity : 0);

    if (requested_total_qty > product_opt->stock_qty) {
        throw exception::ValidationException(
            "Requested quantity exceeds available stock (" + std::to_string(product_opt->stock_qty) + " available)",
            "quantity"
        );
    }

    cart_repo_->AddOrUpdateItem(user_id, req.product_id, req.quantity);
    return BuildCartResponse(user_id);
}

dto::CartResponse CartService::UpdateCartItem(int64_t user_id, int64_t product_id, const dto::UpdateCartItemRequest& req) {
    if (req.quantity <= 0) {
        return RemoveCartItem(user_id, product_id);
    }

    auto product_opt = product_repo_->FindById(product_id);
    if (!product_opt.has_value()) {
        throw exception::NotFoundException("Product not found", "product_id");
    }

    if (req.quantity > product_opt->stock_qty) {
        throw exception::ValidationException(
            "Requested quantity exceeds available stock (" + std::to_string(product_opt->stock_qty) + " available)",
            "quantity"
        );
    }

    cart_repo_->UpdateQuantity(user_id, product_id, req.quantity);
    return BuildCartResponse(user_id);
}

dto::CartResponse CartService::RemoveCartItem(int64_t user_id, int64_t product_id) {
    cart_repo_->RemoveItem(user_id, product_id);
    return BuildCartResponse(user_id);
}

bool CartService::ClearCart(int64_t user_id) {
    return cart_repo_->ClearCart(user_id);
}

int CartService::GetCartItemCount(int64_t user_id) {
    return cart_repo_->GetTotalItemCount(user_id);
}

} // namespace manimart::service
