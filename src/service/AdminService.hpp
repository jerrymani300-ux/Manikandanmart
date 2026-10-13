#pragma once

#include <memory>
#include <vector>
#include "repository/IUserRepository.hpp"
#include "repository/IProductRepository.hpp"
#include "repository/IOrderRepository.hpp"
#include "dto/AdminDto.hpp"
#include "dto/AuthDto.hpp"
#include "dto/ProductDto.hpp"
#include "dto/OrderDto.hpp"

namespace manimart::service {

class AdminService {
public:
    AdminService(
        std::shared_ptr<repository::IUserRepository> user_repo,
        std::shared_ptr<repository::IProductRepository> product_repo,
        std::shared_ptr<repository::IOrderRepository> order_repo
    );

    dto::AdminDashboardStatsResponse GetDashboardStats();
    std::vector<dto::UserResponse> GetAllUsers(int limit = 100, int offset = 0);
    std::vector<dto::ProductResponse> GetAllProducts(int limit = 100, int offset = 0);
    std::vector<dto::OrderResponse> GetAllOrders(int limit = 100, int offset = 0);
    bool ModerateDeleteProduct(int64_t product_id);

private:
    std::shared_ptr<repository::IUserRepository> user_repo_;
    std::shared_ptr<repository::IProductRepository> product_repo_;
    std::shared_ptr<repository::IOrderRepository> order_repo_;
};

} // namespace manimart::service
