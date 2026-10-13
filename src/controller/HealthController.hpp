#pragma once

#include <drogon/HttpController.h>

namespace manimart::controller {

class HealthController : public drogon::HttpController<HealthController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(HealthController::GetHealth, "/api/v1/health", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::CorsFilter");
    METHOD_LIST_END

    void GetHealth(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );
};

} // namespace manimart::controller
