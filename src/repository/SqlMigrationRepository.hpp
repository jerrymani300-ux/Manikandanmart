#pragma once

#include "repository/IMigrationRepository.hpp"
#include "repository/DatabaseConnection.hpp"

namespace manimart::repository {

class SqlMigrationRepository : public IMigrationRepository {
public:
    void EnsureMigrationsTableExists() override;
    std::vector<int> GetAppliedVersions() override;
    bool RecordMigration(int version, const std::string& description) override;
    bool ExecuteSqlScript(const std::string& sql) override;
};

} // namespace manimart::repository
