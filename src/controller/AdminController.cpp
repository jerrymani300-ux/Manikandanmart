#include "controller/AdminController.hpp"
#include "dto/EnvelopeDto.hpp"
#include "exception/AppException.hpp"
#include <drogon/HttpResponse.h>
#include <spdlog/spdlog.h>

namespace manimart::controller {

std::shared_ptr<service::AdminService> AdminController::admin_service_ = nullptr;

void AdminController::SetAdminService(std::shared_ptr<service::AdminService> service) {
    admin_service_ = std::move(service);
}

std::shared_ptr<service::AdminService> AdminController::GetAdminService() {
    return admin_service_;
}

void AdminController::GetDashboard(const drogon::HttpRequestPtr& /*req*/, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!admin_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "AdminService not initialized");
        }

        auto stats = admin_service_->GetDashboardStats();
        nlohmann::json json_res;
        to_json(json_res, stats);

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(json_res).dump());
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

void AdminController::GetUsers(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!admin_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "AdminService not initialized");
        }

        std::string limit_str = req->getParameter("limit");
        std::string offset_str = req->getParameter("offset");
        int limit = limit_str.empty() ? 100 : std::stoi(limit_str);
        int offset = offset_str.empty() ? 0 : std::stoi(offset_str);

        auto users = admin_service_->GetAllUsers(limit, offset);
        nlohmann::json data = nlohmann::json::array();
        for (const auto& u : users) {
            nlohmann::json item;
            to_json(item, u);
            data.push_back(item);
        }

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(data).dump());
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

void AdminController::GetProducts(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!admin_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "AdminService not initialized");
        }

        std::string limit_str = req->getParameter("limit");
        std::string offset_str = req->getParameter("offset");
        int limit = limit_str.empty() ? 100 : std::stoi(limit_str);
        int offset = offset_str.empty() ? 0 : std::stoi(offset_str);

        auto products = admin_service_->GetAllProducts(limit, offset);
        nlohmann::json data = nlohmann::json::array();
        for (const auto& p : products) {
            nlohmann::json item;
            to_json(item, p);
            data.push_back(item);
        }

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(data).dump());
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

void AdminController::GetOrders(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!admin_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "AdminService not initialized");
        }

        std::string limit_str = req->getParameter("limit");
        std::string offset_str = req->getParameter("offset");
        int limit = limit_str.empty() ? 100 : std::stoi(limit_str);
        int offset = offset_str.empty() ? 0 : std::stoi(offset_str);

        auto orders = admin_service_->GetAllOrders(limit, offset);
        nlohmann::json data = nlohmann::json::array();
        for (const auto& o : orders) {
            nlohmann::json item;
            to_json(item, o);
            data.push_back(item);
        }

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(data).dump());
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

void AdminController::ModerateDeleteProduct(const drogon::HttpRequestPtr& /*req*/, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t product_id) {
    try {
        if (!admin_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "AdminService not initialized");
        }

        admin_service_->ModerateDeleteProduct(product_id);

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success().dump());
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
