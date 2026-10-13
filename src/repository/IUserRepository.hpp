#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "model/User.hpp"

namespace manimart::repository {

class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual std::optional<model::User> FindById(int64_t id) = 0;
    virtual std::optional<model::User> FindByEmail(const std::string& email) = 0;
    virtual int64_t Create(const model::User& user) = 0;
    virtual bool Update(const model::User& user) = 0;
    virtual bool Delete(int64_t id) = 0;
    virtual std::vector<model::User> FindAll(int limit = 100, int offset = 0) = 0;
    virtual int CountByRole(model::Role role) = 0;
    virtual int CountAll() = 0;
};

} // namespace manimart::repository
