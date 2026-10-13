#pragma once

#include <drogon/HttpController.h>
#include <memory>
#include "service/AuthService.hpp"

namespace manimart::controller {

class AuthController : public drogon::HttpController<AuthController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::Register, "/api/v1/auth/register", drogon::Post, "manimart::filter::LoggingFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(AuthController::Login, "/api/v1/auth/login", drogon::Post, "manimart::filter::LoggingFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(AuthController::Logout, "/api/v1/auth/logout", drogon::Post, "manimart::filter::LoggingFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(AuthController::GetMe, "/api/v1/auth/me", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::CorsFilter");
    METHOD_LIST_END

    void Register(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void Login(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void Logout(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void GetMe(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    static void SetAuthService(std::shared_ptr<service::AuthService> service);
    static std::shared_ptr<service::AuthService> GetAuthService();

private:
    static std::shared_ptr<service::AuthService> auth_service_;
};

} // namespace manimart::controller
