#pragma once

#include <memory>
#include <vector>
#include "repository/IOrderRepository.hpp"
#include "repository/ICartRepository.hpp"
#include "repository/IProductRepository.hpp"
#include "repository/IUserRepository.hpp"
#include "provider/PaymentStrategy.hpp"
#include "dto/OrderDto.hpp"
#include "model/Order.hpp"

namespace manimart::service {

class OrderService {
public:
    OrderService(
        std::shared_ptr<repository::IOrderRepository> order_repo,
        std::shared_ptr<repository::ICartRepository> cart_repo,
        std::shared_ptr<repository::IProductRepository> product_repo,
        std::shared_ptr<repository::IUserRepository> user_repo,
        std::shared_ptr<provider::IPaymentStrategy> payment_strategy
    );

    dto::OrderResponse Checkout(int64_t buyer_id, const dto::CheckoutRequest& req);
    dto::OrderResponse GetOrderById(int64_t user_id, int64_t order_id, bool is_admin = false);
    std::vector<dto::OrderResponse> GetBuyerOrders(int64_t buyer_id);
    std::vector<dto::OrderResponse> GetSellerOrders(int64_t seller_id);
    std::vector<dto::OrderResponse> GetAllOrders(int limit = 100, int offset = 0);
    dto::OrderResponse UpdateOrderStatus(int64_t user_id, int64_t order_id, const std::string& new_status, bool is_admin = false);
    dto::OrderResponse CancelOrder(int64_t buyer_id, int64_t order_id);

private:
    std::shared_ptr<repository::IOrderRepository> order_repo_;
    std::shared_ptr<repository::ICartRepository> cart_repo_;
    std::shared_ptr<repository::IProductRepository> product_repo_;
    std::shared_ptr<repository::IUserRepository> user_repo_;
    std::shared_ptr<provider::IPaymentStrategy> payment_strategy_;
};

} // namespace manimart::service
