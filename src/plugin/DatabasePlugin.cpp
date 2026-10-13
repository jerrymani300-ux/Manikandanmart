#include "plugin/DatabasePlugin.hpp"
#include "repository/DatabaseConnection.hpp"
#include "repository/RepositoryFactory.hpp"
#include "service/MigrationService.hpp"
#include "util/Env.hpp"
#include <spdlog/spdlog.h>

namespace manimart::plugin {

void DatabasePlugin::initAndStart(const Json::Value& config) {
    spdlog::info("DatabasePlugin: Initializing database connection...");

    std::string db_type = util::Env::Get("DB_TYPE", config.get("type", "sqlite").asString());
    std::string sqlite_path = util::Env::Get("SQLITE_DB_PATH", config.get("sqlite_path", "manimart.db").asString());

    repository::DatabaseConnection::GetInstance().Initialize(db_type, sqlite_path);

    // Run migrations
    auto mig_repo = repository::RepositoryFactory::CreateMigrationRepository();
    service::MigrationService mig_service(mig_repo);
    mig_service.RunMigrations("db/migrations");

    // Check if user table is populated; if empty, run seed.sql
    auto user_repo = repository::RepositoryFactory::CreateUserRepository();
    if (user_repo->CountAll() == 0) {
        spdlog::info("Database is empty, automatically executing db/seed.sql...");
        mig_service.RunSeed("db/seed.sql");
    }

    spdlog::info("DatabasePlugin: Database initialized and ready.");
}

void DatabasePlugin::shutdown() {
    spdlog::info("DatabasePlugin: Shutting down database connection...");
    repository::DatabaseConnection::GetInstance().Close();
}

} // namespace manimart::plugin
