#include "service/AdminService.hpp"
#include "exception/AppException.hpp"
#include "util/Money.hpp"
#include <spdlog/spdlog.h>

namespace manimart::service {

AdminService::AdminService(
    std::shared_ptr<repository::IUserRepository> user_repo,
    std::shared_ptr<repository::IProductRepository> product_repo,
    std::shared_ptr<repository::IOrderRepository> order_repo)
    : user_repo_(std::move(user_repo)),
      product_repo_(std::move(product_repo)),
      order_repo_(std::move(order_repo)) {
    if (!user_repo_ || !product_repo_ || !order_repo_) {
        throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "AdminService: repositories cannot be null");
    }
}

dto::AdminDashboardStatsResponse AdminService::GetDashboardStats() {
    dto::AdminDashboardStatsResponse stats;
    stats.total_users = user_repo_->CountAll();
    stats.total_buyers = user_repo_->CountByRole(model::Role::BUYER);
    stats.total_sellers = user_repo_->CountByRole(model::Role::SELLER);
    stats.total_products = product_repo_->CountAll();
    stats.total_orders = order_repo_->CountAll();

    int64_t gross_sales_cents = order_repo_->GetTotalGrossSalesCents();
    stats.total_gross_sales_cents = gross_sales_cents;
    stats.formatted_gross_sales = util::Money::FromCents(gross_sales_cents).ToFormattedString();

    auto users = user_repo_->FindAll(5, 0);
    for (const auto& u : users) {
        stats.recent_users.push_back(dto::UserDtoBuilder().FromModel(u).Build());
    }

    auto orders = order_repo_->FindAll(5, 0);
    for (const auto& o : orders) {
        stats.recent_orders.push_back(dto::OrderDtoBuilder().FromModel(o).Build());
    }

    return stats;
}

std::vector<dto::UserResponse> AdminService::GetAllUsers(int limit, int offset) {
    auto users = user_repo_->FindAll(limit, offset);
    std::vector<dto::UserResponse> dtos;
    dtos.reserve(users.size());
    for (const auto& u : users) {
        dtos.push_back(dto::UserDtoBuilder().FromModel(u).Build());
    }
    return dtos;
}

std::vector<dto::ProductResponse> AdminService::GetAllProducts(int limit, int offset) {
    auto products = product_repo_->FindAll("", "", limit, offset);
    std::vector<dto::ProductResponse> dtos;
    dtos.reserve(products.size());
    for (const auto& p : products) {
        dtos.push_back(dto::ProductDtoBuilder().FromModel(p).Build());
    }
    return dtos;
}

std::vector<dto::OrderResponse> AdminService::GetAllOrders(int limit, int offset) {
    auto orders = order_repo_->FindAll(limit, offset);
    std::vector<dto::OrderResponse> dtos;
    dtos.reserve(orders.size());
    for (const auto& o : orders) {
        dtos.push_back(dto::OrderDtoBuilder().FromModel(o).Build());
    }
    return dtos;
}

bool AdminService::ModerateDeleteProduct(int64_t product_id) {
    auto product = product_repo_->FindById(product_id);
    if (!product.has_value()) {
        throw exception::NotFoundException("Product not found", "product_id");
    }

    bool success = product_repo_->Delete(product_id);
    if (success) {
        spdlog::warn("ADMIN MODERATION: Product {} ('{}') was removed by administrator", product_id, product->name);
    }
    return success;
}

} // namespace manimart::service
