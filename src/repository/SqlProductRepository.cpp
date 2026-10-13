#include "repository/SqlProductRepository.hpp"
#include <spdlog/spdlog.h>

namespace manimart::repository {

model::Product SqlProductRepository::MapRow(const SqlRow& row) {
    model::Product p;
    p.id = row.GetInt64("id");
    p.seller_id = row.GetInt64("seller_id");
    p.seller_name = row.GetString("seller_name");
    p.name = row.GetString("name");
    p.description = row.GetString("description");
    p.price = util::Money::FromCents(row.GetInt64("price_cents"));
    p.stock_qty = row.GetInt("stock_qty");
    p.category = row.GetString("category");
    p.image_url = row.GetString("image_url");
    p.created_at = row.GetString("created_at");
    p.average_rating = row.GetDouble("average_rating", 0.0);
    p.review_count = row.GetInt("review_count", 0);
    return p;
}

std::optional<model::Product> SqlProductRepository::FindById(int64_t id) {
    const std::string sql = 
        "SELECT p.id, p.seller_id, u.name AS seller_name, p.name, p.description, "
        "       p.price_cents, p.stock_qty, p.category, p.image_url, p.created_at, "
        "       COALESCE(AVG(r.rating), 0.0) AS average_rating, "
        "       COUNT(r.id) AS review_count "
        "FROM products p "
        "JOIN users u ON p.seller_id = u.id "
        "LEFT JOIN reviews r ON p.id = r.product_id "
        "WHERE p.id = ? "
        "GROUP BY p.id, p.seller_id, u.name, p.name, p.description, p.price_cents, p.stock_qty, p.category, p.image_url, p.created_at;";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {id});
    if (rs.IsEmpty()) return std::nullopt;
    return MapRow(rs.Get(0));
}

std::vector<model::Product> SqlProductRepository::FindAll(const std::string& search, const std::string& category, int limit, int offset) {
    std::string sql = 
        "SELECT p.id, p.seller_id, u.name AS seller_name, p.name, p.description, "
        "       p.price_cents, p.stock_qty, p.category, p.image_url, p.created_at, "
        "       COALESCE(AVG(r.rating), 0.0) AS average_rating, "
        "       COUNT(r.id) AS review_count "
        "FROM products p "
        "JOIN users u ON p.seller_id = u.id "
        "LEFT JOIN reviews r ON p.id = r.product_id "
        "WHERE 1=1 ";

    std::vector<SqlParam> params;
    if (!search.empty()) {
        sql += "AND (p.name LIKE ? OR p.description LIKE ?) ";
        std::string pattern = "%" + search + "%";
        params.push_back(pattern);
        params.push_back(pattern);
    }
    if (!category.empty()) {
        sql += "AND p.category = ? ";
        params.push_back(category);
    }

    sql += "GROUP BY p.id, p.seller_id, u.name, p.name, p.description, p.price_cents, p.stock_qty, p.category, p.image_url, p.created_at "
           "ORDER BY p.id DESC LIMIT ? OFFSET ?;";
    params.push_back(limit);
    params.push_back(offset);

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, params);
    std::vector<model::Product> products;
    products.reserve(rs.Size());
    for (const auto& row : rs.GetRows()) {
        products.push_back(MapRow(row));
    }
    return products;
}

std::vector<model::Product> SqlProductRepository::FindBySellerId(int64_t seller_id) {
    const std::string sql = 
        "SELECT p.id, p.seller_id, u.name AS seller_name, p.name, p.description, "
        "       p.price_cents, p.stock_qty, p.category, p.image_url, p.created_at, "
        "       COALESCE(AVG(r.rating), 0.0) AS average_rating, "
        "       COUNT(r.id) AS review_count "
        "FROM products p "
        "JOIN users u ON p.seller_id = u.id "
        "LEFT JOIN reviews r ON p.id = r.product_id "
        "WHERE p.seller_id = ? "
        "GROUP BY p.id, p.seller_id, u.name, p.name, p.description, p.price_cents, p.stock_qty, p.category, p.image_url, p.created_at "
        "ORDER BY p.id DESC;";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {seller_id});
    std::vector<model::Product> products;
    products.reserve(rs.Size());
    for (const auto& row : rs.GetRows()) {
        products.push_back(MapRow(row));
    }
    return products;
}

int64_t SqlProductRepository::Create(const model::Product& product) {
    const std::string sql = 
        "INSERT INTO products (seller_id, name, description, price_cents, stock_qty, category, image_url) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";
    return DatabaseConnection::GetInstance().ExecuteUpdate(sql, {
        product.seller_id,
        product.name,
        product.description,
        product.price.GetCents(),
        product.stock_qty,
        product.category,
        product.image_url
    });
}

bool SqlProductRepository::Update(const model::Product& product) {
    const std::string sql = 
        "UPDATE products SET name = ?, description = ?, price_cents = ?, stock_qty = ?, category = ?, image_url = ? "
        "WHERE id = ? AND seller_id = ?;";
    int64_t changes = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {
        product.name,
        product.description,
        product.price.GetCents(),
        product.stock_qty,
        product.category,
        product.image_url,
        product.id,
        product.seller_id
    });
    return changes > 0;
}

bool SqlProductRepository::UpdateStock(int64_t product_id, int delta_quantity) {
    // Decrement or increment stock safely with check constraint
    const std::string sql = "UPDATE products SET stock_qty = stock_qty + ? WHERE id = ? AND (stock_qty + ?) >= 0;";
    int64_t changes = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {delta_quantity, product_id, delta_quantity});
    return changes > 0;
}

bool SqlProductRepository::Delete(int64_t id) {
    const std::string sql = "DELETE FROM products WHERE id = ?;";
    int64_t changes = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {id});
    return changes > 0;
}

int SqlProductRepository::CountAll() {
    const std::string sql = "SELECT COUNT(*) AS total FROM products;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {});
    if (rs.IsEmpty()) return 0;
    return rs.Get(0).GetInt("total");
}

int SqlProductRepository::CountBySellerId(int64_t seller_id) {
    const std::string sql = "SELECT COUNT(*) AS total FROM products WHERE seller_id = ?;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {seller_id});
    if (rs.IsEmpty()) return 0;
    return rs.Get(0).GetInt("total");
}

std::vector<std::string> SqlProductRepository::GetDistinctCategories() {
    const std::string sql = "SELECT DISTINCT category FROM products ORDER BY category ASC;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {});
    std::vector<std::string> categories;
    for (const auto& row : rs.GetRows()) {
        categories.push_back(row.GetString("category"));
    }
    return categories;
}

} // namespace manimart::repository
