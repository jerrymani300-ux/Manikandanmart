#include "controller/SellerController.hpp"
#include "dto/EnvelopeDto.hpp"
#include "exception/AppException.hpp"
#include <drogon/HttpResponse.h>
#include <spdlog/spdlog.h>

namespace manimart::controller {

std::shared_ptr<service::SellerService> SellerController::seller_service_ = nullptr;
std::shared_ptr<service::ProductService> SellerController::product_service_ = nullptr;
std::shared_ptr<service::OrderService> SellerController::order_service_ = nullptr;

void SellerController::SetServices(
    std::shared_ptr<service::SellerService> seller_service,
    std::shared_ptr<service::ProductService> product_service,
    std::shared_ptr<service::OrderService> order_service) {
    seller_service_ = std::move(seller_service);
    product_service_ = std::move(product_service);
    order_service_ = std::move(order_service);
}

void SellerController::GetProducts(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!product_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "ProductService not initialized");
        }

        auto session = req->session();
        int64_t seller_id = session->get<int64_t>("user_id");

        auto products = product_service_->GetProductsBySeller(seller_id);
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

void SellerController::GetOrders(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!order_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "OrderService not initialized");
        }

        auto session = req->session();
        int64_t seller_id = session->get<int64_t>("user_id");

        auto orders = order_service_->GetSellerOrders(seller_id);
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

void SellerController::GetDashboard(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!seller_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "SellerService not initialized");
        }

        auto session = req->session();
        int64_t seller_id = session->get<int64_t>("user_id");

        auto stats = seller_service_->GetDashboardStats(seller_id);
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

} // namespace manimart::controller
