#include "service/AuthService.hpp"
#include "util/ValidationUtil.hpp"
#include "util/PasswordHasher.hpp"
#include "exception/AppException.hpp"
#include <spdlog/spdlog.h>

namespace manimart::service {

AuthService::AuthService(std::shared_ptr<repository::IUserRepository> user_repo)
    : user_repo_(std::move(user_repo)) {
    if (!user_repo_) {
        throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "AuthService: user_repo cannot be null");
    }
}

dto::UserResponse AuthService::Register(const dto::RegisterRequest& req) {
    // 1. Input Validation
    util::ValidationUtil::ValidateRegistration(req.name, req.email, req.password, req.role);

    std::string email = util::ValidationUtil::Trim(req.email);
    std::string name = util::ValidationUtil::Trim(req.name);

    // 2. Check for duplicate email
    auto existing = user_repo_->FindByEmail(email);
    if (existing.has_value()) {
        throw exception::ConflictException("An account with this email already exists", "email");
    }

    // 3. Hash password using Argon2id
    std::string hash = util::PasswordHasher::HashPassword(req.password);

    // 4. Create User model
    model::User user{
        .name = name,
        .email = email,
        .password_hash = hash,
        .role = model::StringToRole(req.role)
    };

    int64_t id = user_repo_->Create(user);
    if (id <= 0) {
        throw exception::DatabaseException("Failed to create user record");
    }

    user.id = id;
    spdlog::info("Registered new user with id: {}, role: {}", id, req.role);

    return dto::UserDtoBuilder().FromModel(user).Build();
}

dto::UserResponse AuthService::Login(const dto::LoginRequest& req) {
    // 1. Basic validation
    util::ValidationUtil::ValidateLogin(req.email, req.password);

    std::string email = util::ValidationUtil::Trim(req.email);

    // 2. Lookup user by email
    auto user_opt = user_repo_->FindByEmail(email);
    if (!user_opt.has_value()) {
        throw exception::AuthenticationException("Invalid email or password");
    }

    const auto& user = *user_opt;

    // 3. Verify password securely in constant time
    if (!util::PasswordHasher::VerifyPassword(req.password, user.password_hash)) {
        throw exception::AuthenticationException("Invalid email or password");
    }

    spdlog::info("Successful login for user id: {}, role: {}", user.id, model::RoleToString(user.role));
    return dto::UserDtoBuilder().FromModel(user).Build();
}

dto::UserResponse AuthService::GetUserProfile(int64_t user_id) {
    auto user_opt = user_repo_->FindById(user_id);
    if (!user_opt.has_value()) {
        throw exception::NotFoundException("User not found", "user_id");
    }
    return dto::UserDtoBuilder().FromModel(*user_opt).Build();
}

} // namespace manimart::service
