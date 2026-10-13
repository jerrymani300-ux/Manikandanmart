#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "service/AuthService.hpp"
#include "test_helpers/MockRepositories.hpp"
#include "exception/AppException.hpp"
#include "util/PasswordHasher.hpp"

using namespace manimart;
using namespace testing;

class AuthServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_user_repo_ = std::make_shared<test::MockUserRepository>();
        auth_service_ = std::make_unique<service::AuthService>(mock_user_repo_);
    }

    std::shared_ptr<test::MockUserRepository> mock_user_repo_;
    std::unique_ptr<service::AuthService> auth_service_;
};

TEST_F(AuthServiceTest, RegisterSuccessBuyer) {
    dto::RegisterRequest req{
        .name = "Alice Johnson",
        .email = "alice@example.com",
        .password = "SecurePass@123",
        .role = "BUYER"
    };

    EXPECT_CALL(*mock_user_repo_, FindByEmail("alice@example.com"))
        .WillOnce(Return(std::nullopt));

    EXPECT_CALL(*mock_user_repo_, Create(_))
        .WillOnce(Return(101));

    auto res = auth_service_->Register(req);
    EXPECT_EQ(res.id, 101);
    EXPECT_EQ(res.name, "Alice Johnson");
    EXPECT_EQ(res.email, "alice@example.com");
    EXPECT_EQ(res.role, "BUYER");
}

TEST_F(AuthServiceTest, RegisterDuplicateEmailThrowsConflict) {
    dto::RegisterRequest req{
        .name = "Alice",
        .email = "existing@example.com",
        .password = "SecurePass@123",
        .role = "BUYER"
    };

    model::User existing_user{
        .id = 1,
        .name = "Existing User",
        .email = "existing@example.com"
    };

    EXPECT_CALL(*mock_user_repo_, FindByEmail("existing@example.com"))
        .WillOnce(Return(existing_user));

    EXPECT_THROW(auth_service_->Register(req), exception::ConflictException);
}

TEST_F(AuthServiceTest, RegisterInvalidRoleThrowsValidation) {
    dto::RegisterRequest req{
        .name = "Hacker",
        .email = "hacker@example.com",
        .password = "SecurePass@123",
        .role = "ADMIN" // Public admin registration forbidden
    };

    EXPECT_THROW(auth_service_->Register(req), exception::ValidationException);
}

TEST_F(AuthServiceTest, LoginSuccess) {
    std::string password = "MyPassword@123";
    std::string hash = util::PasswordHasher::HashPassword(password);

    model::User db_user{
        .id = 42,
        .name = "Bob Smith",
        .email = "bob@example.com",
        .password_hash = hash,
        .role = model::Role::BUYER
    };

    EXPECT_CALL(*mock_user_repo_, FindByEmail("bob@example.com"))
        .WillOnce(Return(db_user));

    dto::LoginRequest req{
        .email = "bob@example.com",
        .password = password
    };

    auto res = auth_service_->Login(req);
    EXPECT_EQ(res.id, 42);
    EXPECT_EQ(res.name, "Bob Smith");
    EXPECT_EQ(res.role, "BUYER");
}

TEST_F(AuthServiceTest, LoginWrongPasswordThrowsAuthentication) {
    std::string real_pass = "RealPass@123";
    std::string hash = util::PasswordHasher::HashPassword(real_pass);

    model::User db_user{
        .id = 42,
        .name = "Bob Smith",
        .email = "bob@example.com",
        .password_hash = hash,
        .role = model::Role::BUYER
    };

    EXPECT_CALL(*mock_user_repo_, FindByEmail("bob@example.com"))
        .WillOnce(Return(db_user));

    dto::LoginRequest req{
        .email = "bob@example.com",
        .password = "WrongPass@999"
    };

    EXPECT_THROW(auth_service_->Login(req), exception::AuthenticationException);
}
