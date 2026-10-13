#pragma once

#include <memory>
#include <string>
#include <vector>
#include "repository/IMigrationRepository.hpp"

namespace manimart::service {

struct MigrationFileInfo {
    int version{0};
    std::string description;
    std::string filepath;
    std::string content;
};

class MigrationService {
public:
    explicit MigrationService(std::shared_ptr<repository::IMigrationRepository> migration_repo);

    int RunMigrations(const std::string& migrations_dir = "db/migrations");
    bool RunSeed(const std::string& seed_file = "db/seed.sql");

private:
    std::vector<MigrationFileInfo> DiscoverMigrations(const std::string& migrations_dir);
    static std::string ReadFileContent(const std::string& filepath);

    std::shared_ptr<repository::IMigrationRepository> migration_repo_;
};

} // namespace manimart::service
