#include "repository/SqlUserRepository.hpp"
#include <spdlog/spdlog.h>

namespace manimart::repository {

model::User SqlUserRepository::MapRow(const SqlRow& row) {
    model::User u;
    u.id = row.GetInt64("id");
    u.name = row.GetString("name");
    u.email = row.GetString("email");
    u.password_hash = row.GetString("password_hash");
    u.role = model::StringToRole(row.GetString("role"));
    u.created_at = row.GetString("created_at");
    return u;
}

std::optional<model::User> SqlUserRepository::FindById(int64_t id) {
    const std::string sql = "SELECT id, name, email, password_hash, role, created_at FROM users WHERE id = ?;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {id});
    if (rs.IsEmpty()) return std::nullopt;
    return MapRow(rs.Get(0));
}

std::optional<model::User> SqlUserRepository::FindByEmail(const std::string& email) {
    const std::string sql = "SELECT id, name, email, password_hash, role, created_at FROM users WHERE email = ?;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {email});
    if (rs.IsEmpty()) return std::nullopt;
    return MapRow(rs.Get(0));
}

int64_t SqlUserRepository::Create(const model::User& user) {
    const std::string sql = "INSERT INTO users (name, email, password_hash, role) VALUES (?, ?, ?, ?);";
    return DatabaseConnection::GetInstance().ExecuteUpdate(sql, {
        user.name,
        user.email,
        user.password_hash,
        model::RoleToString(user.role)
    });
}

bool SqlUserRepository::Update(const model::User& user) {
    const std::string sql = "UPDATE users SET name = ?, email = ?, password_hash = ?, role = ? WHERE id = ?;";
    int64_t changes = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {
        user.name,
        user.email,
        user.password_hash,
        model::RoleToString(user.role),
        user.id
    });
    return changes > 0;
}

bool SqlUserRepository::Delete(int64_t id) {
    const std::string sql = "DELETE FROM users WHERE id = ?;";
    int64_t changes = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {id});
    return changes > 0;
}

std::vector<model::User> SqlUserRepository::FindAll(int limit, int offset) {
    const std::string sql = "SELECT id, name, email, password_hash, role, created_at FROM users ORDER BY id ASC LIMIT ? OFFSET ?;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {limit, offset});
    std::vector<model::User> users;
    users.reserve(rs.Size());
    for (const auto& row : rs.GetRows()) {
        users.push_back(MapRow(row));
    }
    return users;
}

int SqlUserRepository::CountByRole(model::Role role) {
    const std::string sql = "SELECT COUNT(*) AS total FROM users WHERE role = ?;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {model::RoleToString(role)});
    if (rs.IsEmpty()) return 0;
    return rs.Get(0).GetInt("total");
}

int SqlUserRepository::CountAll() {
    const std::string sql = "SELECT COUNT(*) AS total FROM users;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {});
    if (rs.IsEmpty()) return 0;
    return rs.Get(0).GetInt("total");
}

} // namespace manimart::repository
