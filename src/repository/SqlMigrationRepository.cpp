#include "repository/SqlMigrationRepository.hpp"
#include <spdlog/spdlog.h>

namespace manimart::repository {

void SqlMigrationRepository::EnsureMigrationsTableExists() {
    const std::string sql = 
        "CREATE TABLE IF NOT EXISTS schema_migrations ("
        "    version INTEGER PRIMARY KEY,"
        "    description VARCHAR(255) NOT NULL,"
        "    applied_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");";
    DatabaseConnection::GetInstance().ExecuteBatchScript(sql);
}

std::vector<int> SqlMigrationRepository::GetAppliedVersions() {
    EnsureMigrationsTableExists();
    const std::string sql = "SELECT version FROM schema_migrations ORDER BY version ASC;";
    auto rs = DatabaseConnection::GetInstance().ExecuteQuery(sql, {});
    std::vector<int> versions;
    versions.reserve(rs.Size());
    for (const auto& row : rs.GetRows()) {
        versions.push_back(row.GetInt("version"));
    }
    return versions;
}

bool SqlMigrationRepository::RecordMigration(int version, const std::string& description) {
    const std::string sql = "INSERT INTO schema_migrations (version, description) VALUES (?, ?);";
    int64_t id = DatabaseConnection::GetInstance().ExecuteUpdate(sql, {version, description});
    return id > 0;
}

bool SqlMigrationRepository::ExecuteSqlScript(const std::string& sql) {
    return DatabaseConnection::GetInstance().ExecuteBatchScript(sql);
}

} // namespace manimart::repository
