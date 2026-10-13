#pragma once

#include <string>
#include <vector>
#include <memory>

namespace manimart::repository {

struct MigrationRecord {
    int version{0};
    std::string description;
    std::string applied_at;
};

class IMigrationRepository {
public:
    virtual ~IMigrationRepository() = default;

    virtual void EnsureMigrationsTableExists() = 0;
    virtual std::vector<int> GetAppliedVersions() = 0;
    virtual bool RecordMigration(int version, const std::string& description) = 0;
    virtual bool ExecuteSqlScript(const std::string& sql) = 0;
};

} // namespace manimart::repository
