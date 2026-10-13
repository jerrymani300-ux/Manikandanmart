#pragma once

#include "repository/IWishlistRepository.hpp"
#include "repository/DatabaseConnection.hpp"

namespace manimart::repository {

class SqlWishlistRepository : public IWishlistRepository {
public:
    std::vector<model::WishlistItem> FindByUserId(int64_t user_id) override;
    bool AddItem(int64_t user_id, int64_t product_id) override;
    bool RemoveItem(int64_t user_id, int64_t product_id) override;
    bool IsInWishlist(int64_t user_id, int64_t product_id) override;

private:
    static model::WishlistItem MapRow(const SqlRow& row);
};

} // namespace manimart::repository
