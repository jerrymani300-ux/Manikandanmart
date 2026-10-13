#include "controller/AuthController.hpp"
#include "dto/EnvelopeDto.hpp"
#include "exception/AppException.hpp"
#include <drogon/HttpResponse.h>
#include <spdlog/spdlog.h>

namespace manimart::controller {

std::shared_ptr<service::AuthService> AuthController::auth_service_ = nullptr;

void AuthController::SetAuthService(std::shared_ptr<service::AuthService> service) {
    auth_service_ = std::move(service);
}

std::shared_ptr<service::AuthService> AuthController::GetAuthService() {
    return auth_service_;
}

void AuthController::Register(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!auth_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "AuthService not initialized");
        }

        auto json = nlohmann::json::parse(req->body());
        dto::RegisterRequest reg_req = json.get<dto::RegisterRequest>();

        dto::UserResponse user_res = auth_service_->Register(reg_req);

        // Security: Automatically authenticate newly registered user session
        auto session = req->session();
        if (session) {
            session->erase("user_id");
            session->erase("role");
            session->erase("email");
            session->erase("name");

            session->insert("user_id", user_res.id);
            session->insert("role", user_res.role);
            session->insert("email", user_res.email);
            session->insert("name", user_res.name);

            user_res.token = session->sessionId();
            user_res.session_id = session->sessionId();
        }

        nlohmann::json user_json;
        to_json(user_json, user_res);

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(user_json).dump());
        resp->setStatusCode(drogon::k201Created);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } catch (const exception::AppException& ex) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::FromException(ex).dump());
        resp->setStatusCode(static_cast<drogon::HttpStatusCode>(ex.GetHttpStatus()));
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } catch (const std::exception& ex) {
        spdlog::error("Unexpected error in Register: {}", ex.what());
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            dto::EnvelopeDto::Error("INTERNAL_SERVER_ERROR", "An unexpected error occurred").dump()
        );
        resp->setStatusCode(drogon::k500InternalServerError);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    }
}

void AuthController::Login(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!auth_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "AuthService not initialized");
        }

        auto json = nlohmann::json::parse(req->body());
        dto::LoginRequest login_req = json.get<dto::LoginRequest>();

        dto::UserResponse user_res = auth_service_->Login(login_req);

        // Security: Regenerate / populate authenticated session
        auto session = req->session();
        if (session) {
            session->erase("user_id");
            session->erase("role");
            session->erase("email");
            session->erase("name");

            session->insert("user_id", user_res.id);
            session->insert("role", user_res.role);
            session->insert("email", user_res.email);
            session->insert("name", user_res.name);

            user_res.token = session->sessionId();
            user_res.session_id = session->sessionId();
        }

        nlohmann::json user_json;
        to_json(user_json, user_res);

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(user_json).dump());
        resp->setStatusCode(drogon::k200OK);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } catch (const exception::AppException& ex) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::FromException(ex).dump());
        resp->setStatusCode(static_cast<drogon::HttpStatusCode>(ex.GetHttpStatus()));
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } catch (const std::exception& ex) {
        spdlog::error("Unexpected error in Login: {}", ex.what());
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            dto::EnvelopeDto::Error("INTERNAL_SERVER_ERROR", "An unexpected error occurred").dump()
        );
        resp->setStatusCode(drogon::k500InternalServerError);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    }
}

void AuthController::Logout(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto session = req->session();
    if (session) {
        session->erase("user_id");
        session->erase("role");
        session->erase("email");
        session->erase("name");
    }

    auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success().dump());
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    callback(resp);
}

void AuthController::GetMe(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto session = req->session();
    if (!session || !session->find("user_id")) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            dto::EnvelopeDto::Error("AUTHENTICATION_ERROR", "Not logged in").dump()
        );
        resp->setStatusCode(drogon::k401Unauthorized);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
        return;
    }

    try {
        int64_t user_id = session->get<int64_t>("user_id");
        auto user_res = auth_service_->GetUserProfile(user_id);
        user_res.token = session->sessionId();
        user_res.session_id = session->sessionId();

        nlohmann::json user_json;
        to_json(user_json, user_res);

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(user_json).dump());
        resp->setStatusCode(drogon::k200OK);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } catch (const exception::AppException& ex) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::FromException(ex).dump());
        resp->setStatusCode(static_cast<drogon::HttpStatusCode>(ex.GetHttpStatus()));
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    }
}

} // namespace manimart::controller
