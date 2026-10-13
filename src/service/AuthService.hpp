#pragma once

#include <memory>
#include "repository/IUserRepository.hpp"
#include "dto/AuthDto.hpp"
#include "model/User.hpp"

namespace manimart::service {

class AuthService {
public:
    explicit AuthService(std::shared_ptr<repository::IUserRepository> user_repo);

    dto::UserResponse Register(const dto::RegisterRequest& req);
    dto::UserResponse Login(const dto::LoginRequest& req);
    dto::UserResponse GetUserProfile(int64_t user_id);

private:
    std::shared_ptr<repository::IUserRepository> user_repo_;
};

} // namespace manimart::service
