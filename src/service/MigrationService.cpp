#include "service/MigrationService.hpp"
#include "repository/DatabaseConnection.hpp"
#include "exception/AppException.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <regex>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;

namespace manimart::service {

MigrationService::MigrationService(std::shared_ptr<repository::IMigrationRepository> migration_repo)
    : migration_repo_(std::move(migration_repo)) {
    if (!migration_repo_) {
        throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "MigrationService: migration_repo cannot be null");
    }
}

std::string MigrationService::ReadFileContent(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "Failed to open migration file: " + filepath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<MigrationFileInfo> MigrationService::DiscoverMigrations(const std::string& migrations_dir) {
    std::vector<MigrationFileInfo> migrations;
    if (!fs::exists(migrations_dir)) {
        spdlog::warn("Migrations directory '{}' does not exist", migrations_dir);
        return migrations;
    }

    static const std::regex pattern(R"(^V(\d+)__(.+)\.sql$)");

    for (const auto& entry : fs::directory_iterator(migrations_dir)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            std::smatch match;
            if (std::regex_match(filename, match, pattern)) {
                int version = std::stoi(match[1].str());
                std::string desc = match[2].str();
                // Replace underscores with spaces in description
                std::replace(desc.begin(), desc.end(), '_', ' ');

                migrations.push_back(MigrationFileInfo{
                    .version = version,
                    .description = desc,
                    .filepath = entry.path().string(),
                    .content = ReadFileContent(entry.path().string())
                });
            }
        }
    }

    std::sort(migrations.begin(), migrations.end(), [](const MigrationFileInfo& a, const MigrationFileInfo& b) {
        return a.version < b.version;
    });

    return migrations;
}

int MigrationService::RunMigrations(const std::string& migrations_dir) {
    migration_repo_->EnsureMigrationsTableExists();
    auto applied_versions = migration_repo_->GetAppliedVersions();
    auto available_migrations = DiscoverMigrations(migrations_dir);

    int newly_applied = 0;

    for (const auto& mig : available_migrations) {
        bool already_applied = std::find(applied_versions.begin(), applied_versions.end(), mig.version) != applied_versions.end();
        if (already_applied) {
            continue;
        }

        spdlog::info("Applying migration V{}: {}", mig.version, mig.description);

        bool success = repository::DatabaseConnection::GetInstance().InTransaction<bool>([&]() -> bool {
            if (!migration_repo_->ExecuteSqlScript(mig.content)) {
                throw exception::DatabaseException("Failed executing SQL script for migration V" + std::to_string(mig.version));
            }
            if (!migration_repo_->RecordMigration(mig.version, mig.description)) {
                throw exception::DatabaseException("Failed recording migration V" + std::to_string(mig.version));
            }
            return true;
        });

        if (success) {
            newly_applied++;
            spdlog::info("Migration V{} applied successfully", mig.version);
        }
    }

    spdlog::info("Schema migration check finished. Total newly applied: {}", newly_applied);
    return newly_applied;
}

bool MigrationService::RunSeed(const std::string& seed_file) {
    if (!fs::exists(seed_file)) {
        spdlog::warn("Seed file '{}' not found", seed_file);
        return false;
    }

    std::string seed_sql = ReadFileContent(seed_file);
    spdlog::info("Running database seed from '{}'...", seed_file);
    bool ok = migration_repo_->ExecuteSqlScript(seed_sql);
    if (ok) {
        spdlog::info("Database seed completed successfully.");
    } else {
        spdlog::error("Database seed encountered errors.");
    }
    return ok;
}

} // namespace manimart::service
