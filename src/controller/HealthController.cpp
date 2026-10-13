#include "controller/HealthController.hpp"
#include "repository/DatabaseConnection.hpp"
#include "dto/EnvelopeDto.hpp"
#include <drogon/HttpResponse.h>

namespace manimart::controller {

void HealthController::GetHealth(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback) {

    bool db_healthy = repository::DatabaseConnection::GetInstance().IsHealthy();

    nlohmann::json health_data = {
        {"status", db_healthy ? "UP" : "DEGRADED"},
        {"db", db_healthy ? "UP" : "DOWN"}
    };

    if (db_healthy) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(health_data).dump());
        resp->setStatusCode(drogon::k200OK);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } else {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            dto::EnvelopeDto::Error("DATABASE_ERROR", "Database connection is unhealthy").dump()
        );
        resp->setStatusCode(drogon::k503ServiceUnavailable);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    }
}

} // namespace manimart::controller
