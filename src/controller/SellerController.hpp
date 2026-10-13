#pragma once

#include <drogon/HttpController.h>
#include <memory>
#include "service/SellerService.hpp"
#include "service/ProductService.hpp"
#include "service/OrderService.hpp"

namespace manimart::controller {

class SellerController : public drogon::HttpController<SellerController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(SellerController::GetProducts, "/api/v1/seller/products", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::SellerFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(SellerController::GetOrders, "/api/v1/seller/orders", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::SellerFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(SellerController::GetDashboard, "/api/v1/seller/dashboard", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::SellerFilter", "manimart::filter::CorsFilter");
    METHOD_LIST_END

    void GetProducts(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void GetOrders(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void GetDashboard(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    static void SetServices(
        std::shared_ptr<service::SellerService> seller_service,
        std::shared_ptr<service::ProductService> product_service,
        std::shared_ptr<service::OrderService> order_service
    );

private:
    static std::shared_ptr<service::SellerService> seller_service_;
    static std::shared_ptr<service::ProductService> product_service_;
    static std::shared_ptr<service::OrderService> order_service_;
};

} // namespace manimart::controller
