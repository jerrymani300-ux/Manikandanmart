#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "model/User.hpp"

namespace manimart::dto {

struct RegisterRequest {
    std::string name;
    std::string email;
    std::string password;
    std::string role; // BUYER or SELLER
};

inline void from_json(const nlohmann::json& j, RegisterRequest& req) {
    req.name = j.value("name", "");
    req.email = j.value("email", "");
    req.password = j.value("password", "");
    req.role = j.value("role", "BUYER");
}

struct LoginRequest {
    std::string email;
    std::string password;
};

inline void from_json(const nlohmann::json& j, LoginRequest& req) {
    req.email = j.value("email", "");
    req.password = j.value("password", "");
}

struct UserResponse {
    int64_t id{0};
    std::string name;
    std::string email;
    std::string role;
    std::string token;
    std::string session_id;
    std::string created_at;
};

inline void to_json(nlohmann::json& j, const UserResponse& res) {
    j = {
        {"id", res.id},
        {"name", res.name},
        {"email", res.email},
        {"role", res.role},
        {"token", res.token},
        {"session_id", res.session_id},
        {"created_at", res.created_at}
    };
}

// Builder pattern for UserResponse
class UserDtoBuilder {
public:
    UserDtoBuilder& WithId(int64_t id) {
        dto_.id = id;
        return *this;
    }
    UserDtoBuilder& WithName(std::string name) {
        dto_.name = std::move(name);
        return *this;
    }
    UserDtoBuilder& WithEmail(std::string email) {
        dto_.email = std::move(email);
        return *this;
    }
    UserDtoBuilder& WithRole(std::string role) {
        dto_.role = std::move(role);
        return *this;
    }
    UserDtoBuilder& WithCreatedAt(std::string created_at) {
        dto_.created_at = std::move(created_at);
        return *this;
    }
    UserDtoBuilder& FromModel(const model::User& user) {
        dto_.id = user.id;
        dto_.name = user.name;
        dto_.email = user.email;
        dto_.role = model::RoleToString(user.role);
        dto_.created_at = user.created_at;
        return *this;
    }

    UserResponse Build() const {
        return dto_;
    }

private:
    UserResponse dto_;
};

} // namespace manimart::dto
