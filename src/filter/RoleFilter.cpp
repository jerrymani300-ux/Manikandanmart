#include "filter/RoleFilter.hpp"
#include "dto/EnvelopeDto.hpp"
#include <drogon/HttpResponse.h>

namespace manimart::filter {

void SellerFilter::doFilter(
    const drogon::HttpRequestPtr& req,
    drogon::FilterCallback&& fcb,
    drogon::FilterChainCallback&& fccb) {

    auto session = req->session();
    if (!session || !session->find("user_id")) {
        auto err_json = dto::EnvelopeDto::Error("AUTHENTICATION_ERROR", "Authentication required");
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err_json.dump());
        resp->setStatusCode(drogon::k401Unauthorized);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        fcb(resp);
        return;
    }

    std::string role = session->get<std::string>("role");
    if (role != "SELLER" && role != "ADMIN") {
        auto err_json = dto::EnvelopeDto::Error("AUTHORIZATION_ERROR", "Access restricted to sellers");
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err_json.dump());
        resp->setStatusCode(drogon::k403Forbidden);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        fcb(resp);
        return;
    }

    fccb();
}

void AdminFilter::doFilter(
    const drogon::HttpRequestPtr& req,
    drogon::FilterCallback&& fcb,
    drogon::FilterChainCallback&& fccb) {

    auto session = req->session();
    if (!session || !session->find("user_id")) {
        auto err_json = dto::EnvelopeDto::Error("AUTHENTICATION_ERROR", "Authentication required");
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err_json.dump());
        resp->setStatusCode(drogon::k401Unauthorized);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        fcb(resp);
        return;
    }

    std::string role = session->get<std::string>("role");
    if (role != "ADMIN") {
        auto err_json = dto::EnvelopeDto::Error("AUTHORIZATION_ERROR", "Administrator privileges required");
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err_json.dump());
        resp->setStatusCode(drogon::k403Forbidden);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        fcb(resp);
        return;
    }

    fccb();
}

} // namespace manimart::filter
