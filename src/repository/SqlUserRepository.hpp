#pragma once

#include "repository/IUserRepository.hpp"
#include "repository/DatabaseConnection.hpp"

namespace manimart::repository {

class SqlUserRepository : public IUserRepository {
public:
    std::optional<model::User> FindById(int64_t id) override;
    std::optional<model::User> FindByEmail(const std::string& email) override;
    int64_t Create(const model::User& user) override;
    bool Update(const model::User& user) override;
    bool Delete(int64_t id) override;
    std::vector<model::User> FindAll(int limit = 100, int offset = 0) override;
    int CountByRole(model::Role role) override;
    int CountAll() override;

private:
    static model::User MapRow(const SqlRow& row);
};

} // namespace manimart::repository
