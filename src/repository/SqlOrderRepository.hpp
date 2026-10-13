#pragma once

#include "repository/IOrderRepository.hpp"
#include "repository/DatabaseConnection.hpp"

namespace manimart::repository {

class SqlOrderRepository : public IOrderRepository {
public:
    std::optional<model::Order> FindById(int64_t id) override;
    std::vector<model::Order> FindByBuyerId(int64_t buyer_id) override;
    std::vector<model::Order> FindBySellerId(int64_t seller_id) override;
    std::vector<model::Order> FindAll(int limit = 100, int offset = 0) override;
    int64_t CreateOrder(const model::Order& order) override;
    bool UpdateStatus(int64_t order_id, model::OrderStatus status) override;
    bool HasUserPurchasedProduct(int64_t user_id, int64_t product_id) override;
    int CountAll() override;
    int CountBySellerId(int64_t seller_id) override;
    int64_t GetTotalGrossSalesCents() override;
    int64_t GetSellerRevenueCents(int64_t seller_id) override;

private:
    std::vector<model::OrderItem> FindItemsByOrderId(int64_t order_id);
};

} // namespace manimart::repository
