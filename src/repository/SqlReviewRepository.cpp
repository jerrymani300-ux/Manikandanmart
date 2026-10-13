#include "repository/SqlReviewRepository.hpp"
#include <spdlog/spdlog.h>

namespace manimart::repository {

model::Review SqlReviewRepository::MapRow(const SqlRow& row) {
    model::Review rev;
    rev.id = row.GetInt64("id");
    rev.product_id = row.GetInt64("product_id");
    rev.user_id = row.GetInt64("user_id");
    rev.user_name = row.GetString("user_name");
    rev.rating = row.GetInt("rating");
    rev.comment = row.GetString("comment");
    rev.created_at = row.GetString("created_at");
    return rev;
}

std::vector<model::Review> SqlReviewRepository::FindByProductId(int64_t product_id) {
    const std::string sql = 
        "SELECT r.id, r.product_id, r.user_id, u.name AS user_name, r.rating, r.comment, r.created_at "
        "FROM reviews r "
        "JOIN users u ON r.user_id = u.id "
        "WHERE r.product_id = ? "
        "ORDER BY r.id DESC;";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {product_id});
    std::vector<model::Review> reviews;
    reviews.reserve(rs.Size());
    for (const auto& row : rs.GetRows()) {
        reviews.push_back(MapRow(row));
    }
    return reviews;
}

std::optional<model::Review> SqlReviewRepository::FindByProductAndUser(int64_t product_id, int64_t user_id) {
    const std::string sql = 
        "SELECT r.id, r.product_id, r.user_id, u.name AS user_name, r.rating, r.comment, r.created_at "
        "FROM reviews r "
        "JOIN users u ON r.user_id = u.id "
        "WHERE r.product_id = ? AND r.user_id = ?;";

    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {product_id, user_id});
    if (rs.IsEmpty()) return std::nullopt;
    return MapRow(rs.Get(0));
}

int64_t SqlReviewRepository::Create(const model::Review& review) {
    const std::string sql = "INSERT INTO reviews (product_id, user_id, rating, comment) VALUES (?, ?, ?, ?);";
    return DatabaseConnection::GetInstance().ExecuteUpdate(sql, {
        review.product_id,
        review.user_id,
        review.rating,
        review.comment
    });
}

bool SqlReviewRepository::Delete(int64_t id) {
    const std::string sql = "DELETE FROM reviews WHERE id = ?;";
    int64_t changes = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {id});
    return changes > 0;
}

double SqlReviewRepository::GetAverageRating(int64_t product_id) {
    const std::string sql = "SELECT COALESCE(AVG(rating), 0.0) AS avg_rating FROM reviews WHERE product_id = ?;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {product_id});
    if (rs.IsEmpty()) return 0.0;
    return rs.Get(0).GetDouble("avg_rating", 0.0);
}

int SqlReviewRepository::GetReviewCount(int64_t product_id) {
    const std::string sql = "SELECT COUNT(*) AS total_count FROM reviews WHERE product_id = ?;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {product_id});
    if (rs.IsEmpty()) return 0;
    return rs.Get(0).GetInt("total_count", 0);
}

} // namespace manimart::repository
