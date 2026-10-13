#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace manimart::model {

enum class Role {
    BUYER,
    SELLER,
    ADMIN
};

inline std::string RoleToString(Role role) {
    switch (role) {
        case Role::BUYER: return "BUYER";
        case Role::SELLER: return "SELLER";
        case Role::ADMIN: return "ADMIN";
    }
    return "BUYER";
}

inline Role StringToRole(const std::string& str) {
    if (str == "SELLER") return Role::SELLER;
    if (str == "ADMIN") return Role::ADMIN;
    return Role::BUYER;
}

inline void to_json(nlohmann::json& j, const Role& r) {
    j = RoleToString(r);
}

inline void from_json(const nlohmann::json& j, Role& r) {
    r = StringToRole(j.get<std::string>());
}

} // namespace manimart::model
