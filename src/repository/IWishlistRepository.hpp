#pragma once

#include <vector>
#include <memory>
#include "model/WishlistItem.hpp"

namespace manimart::repository {

class IWishlistRepository {
public:
    virtual ~IWishlistRepository() = default;

    virtual std::vector<model::WishlistItem> FindByUserId(int64_t user_id) = 0;
    virtual bool AddItem(int64_t user_id, int64_t product_id) = 0;
    virtual bool RemoveItem(int64_t user_id, int64_t product_id) = 0;
    virtual bool IsInWishlist(int64_t user_id, int64_t product_id) = 0;
};

} // namespace manimart::repository
