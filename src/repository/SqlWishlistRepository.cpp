#include "repository/SqlWishlistRepository.hpp"
#include <spdlog/spdlog.h>

namespace manimart::repository {

model::WishlistItem SqlWishlistRepository::MapRow(const SqlRow& row) {
    model::WishlistItem item;
    item.id = row.GetInt64("id");
    item.user_id = row.GetInt64("user_id");
    item.product_id = row.GetInt64("product_id");
    item.product_name = row.GetString("product_name");
    item.product_category = row.GetString("product_category");
    item.product_image_url = row.GetString("product_image_url");
    item.product_price = util::Money::FromCents(row.GetInt64("product_price_cents"));
    item.product_stock = row.GetInt("product_stock");
    item.created_at = row.GetString("created_at");
    return item;
}

std::vector<model::WishlistItem> SqlWishlistRepository::FindByUserId(int64_t user_id) {
    const std::string sql = 
        "SELECT w.id, w.user_id, w.product_id, w.created_at, "
        "       p.name AS product_name, p.category AS product_category, "
        "       p.image_url AS product_image_url, p.price_cents AS product_price_cents, "
        "       p.stock_qty AS product_stock "
        "FROM wishlist w "
        "JOIN products p ON w.product_id = p.id "
        "WHERE w.user_id = ? "
        "ORDER BY w.id DESC;";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {user_id});
    std::vector<model::WishlistItem> items;
    items.reserve(rs.Size());
    for (const auto& row : rs.GetRows()) {
        items.push_back(MapRow(row));
    }
    return items;
}

bool SqlWishlistRepository::AddItem(int64_t user_id, int64_t product_id) {
    const std::string sql = "INSERT OR IGNORE INTO wishlist (user_id, product_id) VALUES (?, ?);";
    int64_t changes = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {user_id, product_id});
    return changes > 0;
}

bool SqlWishlistRepository::RemoveItem(int64_t user_id, int64_t product_id) {
    const std::string sql = "DELETE FROM wishlist WHERE user_id = ? AND product_id = ?;";
    int64_t changes = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {user_id, product_id});
    return changes > 0;
}

bool SqlWishlistRepository::IsInWishlist(int64_t user_id, int64_t product_id) {
    const std::string sql = "SELECT COUNT(*) AS total FROM wishlist WHERE user_id = ? AND product_id = ?;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {user_id, product_id});
    if (rs.IsEmpty()) return false;
    return rs.Get(0).GetInt("total") > 0;
}

} // namespace manimart::repository
