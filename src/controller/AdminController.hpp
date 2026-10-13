#pragma once

#include <drogon/HttpController.h>
#include <memory>
#include "service/AdminService.hpp"

namespace manimart::controller {

class AdminController : public drogon::HttpController<AdminController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AdminController::GetDashboard, "/api/v1/admin/dashboard", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::AdminFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(AdminController::GetUsers, "/api/v1/admin/users", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::AdminFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(AdminController::GetProducts, "/api/v1/admin/products", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::AdminFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(AdminController::GetOrders, "/api/v1/admin/orders", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::AdminFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(AdminController::ModerateDeleteProduct, "/api/v1/admin/products/{1}", drogon::Delete, "manimart::filter::LoggingFilter", "manimart::filter::AdminFilter", "manimart::filter::CorsFilter");
    METHOD_LIST_END

    void GetDashboard(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void GetUsers(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void GetProducts(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void GetOrders(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void ModerateDeleteProduct(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t product_id);

    static void SetAdminService(std::shared_ptr<service::AdminService> service);
    static std::shared_ptr<service::AdminService> GetAdminService();

private:
    static std::shared_ptr<service::AdminService> admin_service_;
};

} // namespace manimart::controller
