#include "repository/SqlOrderRepository.hpp"
#include <spdlog/spdlog.h>

namespace manimart::repository {

std::vector<model::OrderItem> SqlOrderRepository::FindItemsByOrderId(int64_t order_id) {
    const std::string sql = 
        "SELECT oi.id, oi.order_id, oi.product_id, oi.quantity, oi.unit_price_cents, oi.created_at, "
        "       p.name AS product_name, p.image_url AS product_image "
        "FROM order_items oi "
        "JOIN products p ON oi.product_id = p.id "
        "WHERE oi.order_id = ? "
        "ORDER BY oi.id ASC;";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {order_id});
    std::vector<model::OrderItem> items;
    items.reserve(rs.Size());
    for (const auto& row : rs.GetRows()) {
        model::OrderItem item;
        item.id = row.GetInt64("id");
        item.order_id = row.GetInt64("order_id");
        item.product_id = row.GetInt64("product_id");
        item.product_name = row.GetString("product_name");
        item.product_image = row.GetString("product_image");
        item.quantity = row.GetInt("quantity");
        item.unit_price = util::Money::FromCents(row.GetInt64("unit_price_cents"));
        item.created_at = row.GetString("created_at");
        items.push_back(item);
    }
    return items;
}

std::optional<model::Order> SqlOrderRepository::FindById(int64_t id) {
    const std::string sql = 
        "SELECT o.id, o.buyer_id, o.status, o.total_amount_cents, o.created_at, "
        "       u.name AS buyer_name, u.email AS buyer_email "
        "FROM orders o "
        "JOIN users u ON o.buyer_id = u.id "
        "WHERE o.id = ?;";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {id});
    if (rs.IsEmpty()) return std::nullopt;

    const auto& row = rs.Get(0);
    model::Order order;
    order.id = row.GetInt64("id");
    order.buyer_id = row.GetInt64("buyer_id");
    order.buyer_name = row.GetString("buyer_name");
    order.buyer_email = row.GetString("buyer_email");
    order.status = model::StringToOrderStatus(row.GetString("status"));
    order.total_amount = util::Money::FromCents(row.GetInt64("total_amount_cents"));
    order.created_at = row.GetString("created_at");
    order.items = FindItemsByOrderId(order.id);
    return order;
}

std::vector<model::Order> SqlOrderRepository::FindByBuyerId(int64_t buyer_id) {
    const std::string sql = 
        "SELECT o.id, o.buyer_id, o.status, o.total_amount_cents, o.created_at, "
        "       u.name AS buyer_name, u.email AS buyer_email "
        "FROM orders o "
        "JOIN users u ON o.buyer_id = u.id "
        "WHERE o.buyer_id = ? "
        "ORDER BY o.id DESC;";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {buyer_id});
    std::vector<model::Order> orders;
    orders.reserve(rs.Size());
    for (const auto& row : rs.GetRows()) {
        model::Order order;
        order.id = row.GetInt64("id");
        order.buyer_id = row.GetInt64("buyer_id");
        order.buyer_name = row.GetString("buyer_name");
        order.buyer_email = row.GetString("buyer_email");
        order.status = model::StringToOrderStatus(row.GetString("status"));
        order.total_amount = util::Money::FromCents(row.GetInt64("total_amount_cents"));
        order.created_at = row.GetString("created_at");
        order.items = FindItemsByOrderId(order.id);
        orders.push_back(order);
    }
    return orders;
}

std::vector<model::Order> SqlOrderRepository::FindBySellerId(int64_t seller_id) {
    const std::string sql = 
        "SELECT DISTINCT o.id, o.buyer_id, o.status, o.total_amount_cents, o.created_at, "
        "       u.name AS buyer_name, u.email AS buyer_email "
        "FROM orders o "
        "JOIN users u ON o.buyer_id = u.id "
        "JOIN order_items oi ON o.id = oi.order_id "
        "JOIN products p ON oi.product_id = p.id "
        "WHERE p.seller_id = ? "
        "ORDER BY o.id DESC;";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {seller_id});
    std::vector<model::Order> orders;
    orders.reserve(rs.Size());
    for (const auto& row : rs.GetRows()) {
        model::Order order;
        order.id = row.GetInt64("id");
        order.buyer_id = row.GetInt64("buyer_id");
        order.buyer_name = row.GetString("buyer_name");
        order.buyer_email = row.GetString("buyer_email");
        order.status = model::StringToOrderStatus(row.GetString("status"));
        order.total_amount = util::Money::FromCents(row.GetInt64("total_amount_cents"));
        order.created_at = row.GetString("created_at");
        order.items = FindItemsByOrderId(order.id);
        orders.push_back(order);
    }
    return orders;
}

std::vector<model::Order> SqlOrderRepository::FindAll(int limit, int offset) {
    const std::string sql = 
        "SELECT o.id, o.buyer_id, o.status, o.total_amount_cents, o.created_at, "
        "       u.name AS buyer_name, u.email AS buyer_email "
        "FROM orders o "
        "JOIN users u ON o.buyer_id = u.id "
        "ORDER BY o.id DESC LIMIT ? OFFSET ?;";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {limit, offset});
    std::vector<model::Order> orders;
    orders.reserve(rs.Size());
    for (const auto& row : rs.GetRows()) {
        model::Order order;
        order.id = row.GetInt64("id");
        order.buyer_id = row.GetInt64("buyer_id");
        order.buyer_name = row.GetString("buyer_name");
        order.buyer_email = row.GetString("buyer_email");
        order.status = model::StringToOrderStatus(row.GetString("status"));
        order.total_amount = util::Money::FromCents(row.GetInt64("total_amount_cents"));
        order.created_at = row.GetString("created_at");
        order.items = FindItemsByOrderId(order.id);
        orders.push_back(order);
    }
    return orders;
}

int64_t SqlOrderRepository::CreateOrder(const model::Order& order) {
    // Insert order record
    const std::string sql = 
        "INSERT INTO orders (buyer_id, status, total_amount_cents) VALUES (?, ?, ?);";
    int64_t order_id = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {
        order.buyer_id,
        model::OrderStatusToString(order.status),
        order.total_amount.GetCents()
    });

    // Insert order items
    const std::string item_sql = 
        "INSERT INTO order_items (order_id, product_id, quantity, unit_price_cents) VALUES (?, ?, ?, ?);";
    for (const auto& item : order.items) {
        DatabaseConnection::GetInstance().ExecuteUpdate(item_sql, {
            order_id,
            item.product_id,
            item.quantity,
            item.unit_price.GetCents()
        });
    }

    return order_id;
}

bool SqlOrderRepository::UpdateStatus(int64_t order_id, model::OrderStatus status) {
    const std::string sql = "UPDATE orders SET status = ? WHERE id = ?;";
    int64_t changes = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {
        model::OrderStatusToString(status),
        order_id
    });
    return changes > 0;
}

bool SqlOrderRepository::HasUserPurchasedProduct(int64_t user_id, int64_t product_id) {
    const std::string sql = 
        "SELECT COUNT(oi.id) AS total "
        "FROM order_items oi "
        "JOIN orders o ON oi.order_id = o.id "
        "WHERE o.buyer_id = ? AND oi.product_id = ? AND o.status = 'DELIVERED';";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {user_id, product_id});
    if (rs.IsEmpty()) return false;
    return rs.Get(0).GetInt("total") > 0;
}

int SqlOrderRepository::CountAll() {
    const std::string sql = "SELECT COUNT(*) AS total FROM orders;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {});
    if (rs.IsEmpty()) return 0;
    return rs.Get(0).GetInt("total");
}

int SqlOrderRepository::CountBySellerId(int64_t seller_id) {
    const std::string sql = 
        "SELECT COUNT(DISTINCT o.id) AS total "
        "FROM orders o "
        "JOIN order_items oi ON o.id = oi.order_id "
        "JOIN products p ON oi.product_id = p.id "
        "WHERE p.seller_id = ?;";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {seller_id});
    if (rs.IsEmpty()) return 0;
    return rs.Get(0).GetInt("total");
}

int64_t SqlOrderRepository::GetTotalGrossSalesCents() {
    const std::string sql = "SELECT COALESCE(SUM(total_amount_cents), 0) AS total_sales FROM orders WHERE status != 'CANCELLED';";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {});
    if (rs.IsEmpty()) return 0;
    return rs.Get(0).GetInt64("total_sales");
}

int64_t SqlOrderRepository::GetSellerRevenueCents(int64_t seller_id) {
    const std::string sql = 
        "SELECT COALESCE(SUM(oi.quantity * oi.unit_price_cents), 0) AS revenue "
        "FROM order_items oi "
        "JOIN orders o ON oi.order_id = o.id "
        "JOIN products p ON oi.product_id = p.id "
        "WHERE p.seller_id = ? AND o.status != 'CANCELLED';";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {seller_id});
    if (rs.IsEmpty()) return 0;
    return rs.Get(0).GetInt64("revenue");
}

} // namespace manimart::repository
