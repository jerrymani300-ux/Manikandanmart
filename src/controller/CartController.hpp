#pragma once

#include <drogon/HttpController.h>
#include <memory>
#include "service/CartService.hpp"

namespace manimart::controller {

class CartController : public drogon::HttpController<CartController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(CartController::GetCart, "/api/v1/cart", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::AuthFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(CartController::AddToCart, "/api/v1/cart/items", drogon::Post, "manimart::filter::LoggingFilter", "manimart::filter::AuthFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(CartController::UpdateCartItem, "/api/v1/cart/items/{1}", drogon::Put, "manimart::filter::LoggingFilter", "manimart::filter::AuthFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(CartController::RemoveCartItem, "/api/v1/cart/items/{1}", drogon::Delete, "manimart::filter::LoggingFilter", "manimart::filter::AuthFilter", "manimart::filter::CorsFilter");
    METHOD_LIST_END

    void GetCart(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void AddToCart(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void UpdateCartItem(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t product_id);
    void RemoveCartItem(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t product_id);

    static void SetCartService(std::shared_ptr<service::CartService> service);
    static std::shared_ptr<service::CartService> GetCartService();

private:
    static std::shared_ptr<service::CartService> cart_service_;
};

} // namespace manimart::controller
