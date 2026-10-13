#pragma once

#include <cstdint>
#include <string>
#include "model/Role.hpp"

namespace manimart::model {

struct User {
    int64_t id{0};
    std::string name;
    std::string email;
    std::string password_hash;
    Role role{Role::BUYER};
    std::string created_at;

    [[nodiscard]] bool IsAdmin() const noexcept { return role == Role::ADMIN; }
    [[nodiscard]] bool IsSeller() const noexcept { return role == Role::SELLER; }
    [[nodiscard]] bool IsBuyer() const noexcept { return role == Role::BUYER; }
};

} // namespace manimart::model
