#pragma once

#include <drogon/HttpController.h>
#include <memory>
#include "service/OrderService.hpp"

namespace manimart::controller {

class OrderController : public drogon::HttpController<OrderController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(OrderController::Checkout, "/api/v1/orders", drogon::Post, "manimart::filter::LoggingFilter", "manimart::filter::AuthFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(OrderController::GetOrders, "/api/v1/orders", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::AuthFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(OrderController::GetOrderById, "/api/v1/orders/{1}", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::AuthFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(OrderController::CancelOrder, "/api/v1/orders/{1}/cancel", drogon::Post, "manimart::filter::LoggingFilter", "manimart::filter::AuthFilter", "manimart::filter::CorsFilter");
    METHOD_LIST_END

    void Checkout(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void GetOrders(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void GetOrderById(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t id);
    void CancelOrder(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t id);

    static void SetOrderService(std::shared_ptr<service::OrderService> service);
    static std::shared_ptr<service::OrderService> GetOrderService();

private:
    static std::shared_ptr<service::OrderService> order_service_;
};

} // namespace manimart::controller
