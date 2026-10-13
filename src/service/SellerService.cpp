#include "service/SellerService.hpp"
#include "exception/AppException.hpp"
#include "util/Money.hpp"

namespace manimart::service {

SellerService::SellerService(
    std::shared_ptr<repository::IProductRepository> product_repo,
    std::shared_ptr<repository::IOrderRepository> order_repo)
    : product_repo_(std::move(product_repo)), order_repo_(std::move(order_repo)) {
    if (!product_repo_ || !order_repo_) {
        throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "SellerService: repositories cannot be null");
    }
}

dto::SellerDashboardStatsResponse SellerService::GetDashboardStats(int64_t seller_id) {
    dto::SellerDashboardStatsResponse stats;
    stats.total_products = product_repo_->CountBySellerId(seller_id);
    stats.total_orders = order_repo_->CountBySellerId(seller_id);

    int64_t rev_cents = order_repo_->GetSellerRevenueCents(seller_id);
    stats.total_revenue_cents = rev_cents;
    stats.formatted_revenue = util::Money::FromCents(rev_cents).ToFormattedString();

    auto orders = order_repo_->FindBySellerId(seller_id);
    int pending_count = 0;
    for (const auto& ord : orders) {
        if (ord.status == model::OrderStatus::PENDING || ord.status == model::OrderStatus::CONFIRMED) {
            pending_count++;
        }
    }
    stats.pending_orders = pending_count;

    // Top products
    auto products = product_repo_->FindBySellerId(seller_id);
    for (size_t i = 0; i < products.size() && i < 5; ++i) {
        stats.top_products.push_back(dto::ProductDtoBuilder().FromModel(products[i]).Build());
    }

    // Recent orders
    for (size_t i = 0; i < orders.size() && i < 5; ++i) {
        stats.recent_orders.push_back(dto::OrderDtoBuilder().FromModel(orders[i]).Build());
    }

    return stats;
}

} // namespace manimart::service
