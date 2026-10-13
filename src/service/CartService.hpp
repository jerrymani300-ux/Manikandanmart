#pragma once

#include <memory>
#include <vector>
#include "repository/ICartRepository.hpp"
#include "repository/IProductRepository.hpp"
#include "dto/CartDto.hpp"

namespace manimart::service {

class CartService {
public:
    CartService(
        std::shared_ptr<repository::ICartRepository> cart_repo,
        std::shared_ptr<repository::IProductRepository> product_repo
    );

    dto::CartResponse GetUserCart(int64_t user_id);
    dto::CartResponse AddToCart(int64_t user_id, const dto::AddToCartRequest& req);
    dto::CartResponse UpdateCartItem(int64_t user_id, int64_t product_id, const dto::UpdateCartItemRequest& req);
    dto::CartResponse RemoveCartItem(int64_t user_id, int64_t product_id);
    bool ClearCart(int64_t user_id);
    int GetCartItemCount(int64_t user_id);

private:
    dto::CartResponse BuildCartResponse(int64_t user_id);

    std::shared_ptr<repository::ICartRepository> cart_repo_;
    std::shared_ptr<repository::IProductRepository> product_repo_;
};

} // namespace manimart::service
