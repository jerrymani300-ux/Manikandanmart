#include "repository/SqlCartRepository.hpp"
#include <spdlog/spdlog.h>

namespace manimart::repository {

model::CartItem SqlCartRepository::MapRow(const SqlRow& row) {
    model::CartItem item;
    item.id = row.GetInt64("id");
    item.user_id = row.GetInt64("user_id");
    item.product_id = row.GetInt64("product_id");
    item.product_name = row.GetString("product_name");
    item.product_category = row.GetString("product_category");
    item.product_image_url = row.GetString("product_image_url");
    item.product_price = util::Money::FromCents(row.GetInt64("product_price_cents"));
    item.product_stock = row.GetInt("product_stock");
    item.quantity = row.GetInt("quantity");
    item.created_at = row.GetString("created_at");
    return item;
}

std::vector<model::CartItem> SqlCartRepository::FindByUserId(int64_t user_id) {
    const std::string sql = 
        "SELECT c.id, c.user_id, c.product_id, c.quantity, c.created_at, "
        "       p.name AS product_name, p.category AS product_category, "
        "       p.image_url AS product_image_url, p.price_cents AS product_price_cents, "
        "       p.stock_qty AS product_stock "
        "FROM cart_items c "
        "JOIN products p ON c.product_id = p.id "
        "WHERE c.user_id = ? "
        "ORDER BY c.id ASC;";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {user_id});
    std::vector<model::CartItem> items;
    items.reserve(rs.Size());
    for (const auto& row : rs.GetRows()) {
        items.push_back(MapRow(row));
    }
    return items;
}

std::optional<model::CartItem> SqlCartRepository::FindByUserAndProduct(int64_t user_id, int64_t product_id) {
    const std::string sql = 
        "SELECT c.id, c.user_id, c.product_id, c.quantity, c.created_at, "
        "       p.name AS product_name, p.category AS product_category, "
        "       p.image_url AS product_image_url, p.price_cents AS product_price_cents, "
        "       p.stock_qty AS product_stock "
        "FROM cart_items c "
        "JOIN products p ON c.product_id = p.id "
        "WHERE c.user_id = ? AND c.product_id = ?;";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {user_id, product_id});
    if (rs.IsEmpty()) return std::nullopt;
    return MapRow(rs.Get(0));
}

bool SqlCartRepository::AddOrUpdateItem(int64_t user_id, int64_t product_id, int quantity) {
    auto existing = FindByUserAndProduct(user_id, product_id);
    if (existing.has_value()) {
        return UpdateQuantity(user_id, product_id, existing->quantity + quantity);
    }

    const std::string sql = "INSERT INTO cart_items (user_id, product_id, quantity) VALUES (?, ?, ?);";
    int64_t id = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {user_id, product_id, quantity});
    return id > 0;
}

bool SqlCartRepository::UpdateQuantity(int64_t user_id, int64_t product_id, int quantity) {
    if (quantity <= 0) {
        return RemoveItem(user_id, product_id);
    }
    const std::string sql = "UPDATE cart_items SET quantity = ? WHERE user_id = ? AND product_id = ?;";
    int64_t changes = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {quantity, user_id, product_id});
    return changes > 0;
}

bool SqlCartRepository::RemoveItem(int64_t user_id, int64_t product_id) {
    const std::string sql = "DELETE FROM cart_items WHERE user_id = ? AND product_id = ?;";
    int64_t changes = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {user_id, product_id});
    return changes > 0;
}

bool SqlCartRepository::ClearCart(int64_t user_id) {
    const std::string sql = "DELETE FROM cart_items WHERE user_id = ?;";
    DatabaseConnection::GetInstance().ExecuteUpdate(sql, {user_id});
    return true;
}

int SqlCartRepository::GetTotalItemCount(int64_t user_id) {
    const std::string sql = "SELECT COALESCE(SUM(quantity), 0) AS total_count FROM cart_items WHERE user_id = ?;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {user_id});
    if (rs.IsEmpty()) return 0;
    return rs.Get(0).GetInt("total_count");
}

} // namespace manimart::repository
