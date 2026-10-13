#pragma once

#include <drogon/HttpController.h>
#include <memory>
#include "service/ProductService.hpp"

namespace manimart::controller {

class ProductController : public drogon::HttpController<ProductController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ProductController::GetProducts, "/api/v1/products", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(ProductController::GetProductById, "/api/v1/products/{1}", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(ProductController::CreateProduct, "/api/v1/products", drogon::Post, "manimart::filter::LoggingFilter", "manimart::filter::SellerFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(ProductController::UpdateProduct, "/api/v1/products/{1}", drogon::Put, "manimart::filter::LoggingFilter", "manimart::filter::SellerFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(ProductController::DeleteProduct, "/api/v1/products/{1}", drogon::Delete, "manimart::filter::LoggingFilter", "manimart::filter::SellerFilter", "manimart::filter::CorsFilter");
    METHOD_LIST_END

    void GetProducts(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void GetProductById(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t id);
    void CreateProduct(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void UpdateProduct(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t id);
    void DeleteProduct(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t id);

    static void SetProductService(std::shared_ptr<service::ProductService> service);
    static std::shared_ptr<service::ProductService> GetProductService();

private:
    static std::shared_ptr<service::ProductService> product_service_;
};

} // namespace manimart::controller
