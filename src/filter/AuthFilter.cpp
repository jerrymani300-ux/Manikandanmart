#include "filter/AuthFilter.hpp"
#include "dto/EnvelopeDto.hpp"
#include <drogon/HttpResponse.h>

namespace manimart::filter {

void AuthFilter::doFilter(
    const drogon::HttpRequestPtr& req,
    drogon::FilterCallback&& fcb,
    drogon::FilterChainCallback&& fccb) {

    auto session = req->session();
    if (!session || !session->find("user_id")) {
        auto err_json = dto::EnvelopeDto::Error(
            "AUTHENTICATION_ERROR",
            "Authentication required. Please log in to proceed."
        );
        auto resp = drogon::HttpResponse::newHttpJsonResponse(err_json.dump());
        resp->setStatusCode(drogon::k401Unauthorized);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        fcb(resp);
        return;
    }

    fccb();
}

} // namespace manimart::filter
