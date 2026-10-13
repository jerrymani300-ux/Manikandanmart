#include "controller/OrderController.hpp"
#include "dto/EnvelopeDto.hpp"
#include "exception/AppException.hpp"
#include <drogon/HttpResponse.h>
#include <spdlog/spdlog.h>

namespace manimart::controller {

std::shared_ptr<service::OrderService> OrderController::order_service_ = nullptr;

void OrderController::SetOrderService(std::shared_ptr<service::OrderService> service) {
    order_service_ = std::move(service);
}

std::shared_ptr<service::OrderService> OrderController::GetOrderService() {
    return order_service_;
}

void OrderController::Checkout(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!order_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "OrderService not initialized");
        }

        auto session = req->session();
        int64_t buyer_id = session->get<int64_t>("user_id");

        dto::CheckoutRequest checkout_req;
        if (!req->body().empty()) {
            auto json = nlohmann::json::parse(req->body());
            checkout_req = json.get<dto::CheckoutRequest>();
        }

        auto order_res = order_service_->Checkout(buyer_id, checkout_req);
        nlohmann::json json_res;
        to_json(json_res, order_res);

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(json_res).dump());
        resp->setStatusCode(drogon::k201Created);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } catch (const exception::AppException& ex) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::FromException(ex).dump());
        resp->setStatusCode(static_cast<drogon::HttpStatusCode>(ex.GetHttpStatus()));
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } catch (const std::exception& ex) {
        spdlog::error("Error in Checkout: {}", ex.what());
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            dto::EnvelopeDto::Error("INTERNAL_SERVER_ERROR", "Checkout failed: " + std::string(ex.what())).dump()
        );
        resp->setStatusCode(drogon::k500InternalServerError);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    }
}

void OrderController::GetOrders(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!order_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "OrderService not initialized");
        }

        auto session = req->session();
        int64_t buyer_id = session->get<int64_t>("user_id");

        auto orders = order_service_->GetBuyerOrders(buyer_id);
        nlohmann::json data = nlohmann::json::array();
        for (const auto& ord : orders) {
            nlohmann::json item;
            to_json(item, ord);
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

void OrderController::GetOrderById(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t id) {
    try {
        if (!order_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "OrderService not initialized");
        }

        auto session = req->session();
        int64_t user_id = session->get<int64_t>("user_id");
        std::string role = session->get<std::string>("role");
        bool is_admin = (role == "ADMIN");

        auto order_res = order_service_->GetOrderById(user_id, id, is_admin);
        nlohmann::json json_res;
        to_json(json_res, order_res);

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

void OrderController::CancelOrder(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t id) {
    try {
        if (!order_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "OrderService not initialized");
        }

        auto session = req->session();
        int64_t user_id = session->get<int64_t>("user_id");

        auto order_res = order_service_->CancelOrder(user_id, id);
        nlohmann::json json_res;
        to_json(json_res, order_res);

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
