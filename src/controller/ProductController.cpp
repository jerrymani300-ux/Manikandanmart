#include "controller/ProductController.hpp"
#include "dto/EnvelopeDto.hpp"
#include "exception/AppException.hpp"
#include <drogon/HttpResponse.h>
#include <spdlog/spdlog.h>

namespace manimart::controller {

std::shared_ptr<service::ProductService> ProductController::product_service_ = nullptr;

void ProductController::SetProductService(std::shared_ptr<service::ProductService> service) {
    product_service_ = std::move(service);
}

std::shared_ptr<service::ProductService> ProductController::GetProductService() {
    return product_service_;
}

void ProductController::GetProducts(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!product_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "ProductService not initialized");
        }

        std::string search = req->getParameter("search");
        std::string category = req->getParameter("category");
        std::string limit_str = req->getParameter("limit");
        std::string offset_str = req->getParameter("offset");

        int limit = limit_str.empty() ? 100 : std::stoi(limit_str);
        int offset = offset_str.empty() ? 0 : std::stoi(offset_str);

        auto products = product_service_->GetProducts(search, category, limit, offset);

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
    } catch (const std::exception& ex) {
        spdlog::error("Error in GetProducts: {}", ex.what());
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            dto::EnvelopeDto::Error("INTERNAL_SERVER_ERROR", "Error retrieving products").dump()
        );
        resp->setStatusCode(drogon::k500InternalServerError);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    }
}

void ProductController::GetProductById(const drogon::HttpRequestPtr& /*req*/, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t id) {
    try {
        if (!product_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "ProductService not initialized");
        }

        auto product = product_service_->GetProductById(id);
        nlohmann::json item;
        to_json(item, product);

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(item).dump());
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

void ProductController::CreateProduct(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!product_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "ProductService not initialized");
        }

        auto session = req->session();
        int64_t seller_id = session->get<int64_t>("user_id");

        auto json = nlohmann::json::parse(req->body());
        dto::CreateProductRequest create_req = json.get<dto::CreateProductRequest>();

        auto product = product_service_->CreateProduct(seller_id, create_req);
        nlohmann::json item;
        to_json(item, product);

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(item).dump());
        resp->setStatusCode(drogon::k201Created);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } catch (const exception::AppException& ex) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::FromException(ex).dump());
        resp->setStatusCode(static_cast<drogon::HttpStatusCode>(ex.GetHttpStatus()));
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    }
}

void ProductController::UpdateProduct(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t id) {
    try {
        if (!product_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "ProductService not initialized");
        }

        auto session = req->session();
        int64_t seller_id = session->get<int64_t>("user_id");
        std::string role = session->get<std::string>("role");
        bool is_admin = (role == "ADMIN");

        auto json = nlohmann::json::parse(req->body());
        dto::UpdateProductRequest update_req = json.get<dto::UpdateProductRequest>();

        auto product = product_service_->UpdateProduct(seller_id, id, update_req, is_admin);
        nlohmann::json item;
        to_json(item, product);

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(item).dump());
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

void ProductController::DeleteProduct(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t id) {
    try {
        if (!product_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "ProductService not initialized");
        }

        auto session = req->session();
        int64_t seller_id = session->get<int64_t>("user_id");
        std::string role = session->get<std::string>("role");
        bool is_admin = (role == "ADMIN");

        product_service_->DeleteProduct(seller_id, id, is_admin);

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
