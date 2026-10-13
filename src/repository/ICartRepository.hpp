#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "model/CartItem.hpp"

namespace manimart::repository {

class ICartRepository {
public:
    virtual ~ICartRepository() = default;

    virtual std::vector<model::CartItem> FindByUserId(int64_t user_id) = 0;
    virtual std::optional<model::CartItem> FindByUserAndProduct(int64_t user_id, int64_t product_id) = 0;
    virtual bool AddOrUpdateItem(int64_t user_id, int64_t product_id, int quantity) = 0;
    virtual bool UpdateQuantity(int64_t user_id, int64_t product_id, int quantity) = 0;
    virtual bool RemoveItem(int64_t user_id, int64_t product_id) = 0;
    virtual bool ClearCart(int64_t user_id) = 0;
    virtual int GetTotalItemCount(int64_t user_id) = 0;
};

} // namespace manimart::repository
