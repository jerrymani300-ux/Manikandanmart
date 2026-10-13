#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "model/Order.hpp"
#include "model/OrderItem.hpp"

namespace manimart::repository {

class IOrderRepository {
public:
    virtual ~IOrderRepository() = default;

    virtual std::optional<model::Order> FindById(int64_t id) = 0;
    virtual std::vector<model::Order> FindByBuyerId(int64_t buyer_id) = 0;
    virtual std::vector<model::Order> FindBySellerId(int64_t seller_id) = 0;
    virtual std::vector<model::Order> FindAll(int limit = 100, int offset = 0) = 0;
    virtual int64_t CreateOrder(const model::Order& order) = 0;
    virtual bool UpdateStatus(int64_t order_id, model::OrderStatus status) = 0;
    virtual bool HasUserPurchasedProduct(int64_t user_id, int64_t product_id) = 0;
    virtual int CountAll() = 0;
    virtual int CountBySellerId(int64_t seller_id) = 0;
    virtual int64_t GetTotalGrossSalesCents() = 0;
    virtual int64_t GetSellerRevenueCents(int64_t seller_id) = 0;
};

} // namespace manimart::repository
