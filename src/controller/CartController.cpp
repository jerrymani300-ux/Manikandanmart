#include "controller/CartController.hpp"
#include "dto/EnvelopeDto.hpp"
#include "exception/AppException.hpp"
#include <drogon/HttpResponse.h>
#include <spdlog/spdlog.h>

namespace manimart::controller {

std::shared_ptr<service::CartService> CartController::cart_service_ = nullptr;

void CartController::SetCartService(std::shared_ptr<service::CartService> service) {
    cart_service_ = std::move(service);
}

std::shared_ptr<service::CartService> CartController::GetCartService() {
    return cart_service_;
}

void CartController::GetCart(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!cart_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "CartService not initialized");
        }

        auto session = req->session();
        int64_t user_id = session->get<int64_t>("user_id");

        auto cart_res = cart_service_->GetUserCart(user_id);
        nlohmann::json json_res;
        to_json(json_res, cart_res);

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

void CartController::AddToCart(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!cart_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "CartService not initialized");
        }

        auto session = req->session();
        int64_t user_id = session->get<int64_t>("user_id");

        auto json = nlohmann::json::parse(req->body());
        dto::AddToCartRequest add_req = json.get<dto::AddToCartRequest>();

        auto cart_res = cart_service_->AddToCart(user_id, add_req);
        nlohmann::json json_res;
        to_json(json_res, cart_res);

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

void CartController::UpdateCartItem(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t product_id) {
    try {
        if (!cart_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "CartService not initialized");
        }

        auto session = req->session();
        int64_t user_id = session->get<int64_t>("user_id");

        auto json = nlohmann::json::parse(req->body());
        dto::UpdateCartItemRequest update_req = json.get<dto::UpdateCartItemRequest>();

        auto cart_res = cart_service_->UpdateCartItem(user_id, product_id, update_req);
        nlohmann::json json_res;
        to_json(json_res, cart_res);

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

void CartController::RemoveCartItem(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t product_id) {
    try {
        if (!cart_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "CartService not initialized");
        }

        auto session = req->session();
        int64_t user_id = session->get<int64_t>("user_id");

        auto cart_res = cart_service_->RemoveCartItem(user_id, product_id);
        nlohmann::json json_res;
        to_json(json_res, cart_res);

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
