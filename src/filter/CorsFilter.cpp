#include "filter/CorsFilter.hpp"
#include <drogon/HttpResponse.h>

namespace manimart::filter {

void CorsFilter::doFilter(
    const drogon::HttpRequestPtr& req,
    drogon::FilterCallback&& fcb,
    drogon::FilterChainCallback&& fccb) {

    if (req->method() == drogon::Options) {
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With, X-Request-ID");
        resp->addHeader("Access-Control-Allow-Credentials", "true");
        resp->setStatusCode(drogon::k200OK);
        fcb(resp);
        return;
    }

    fccb();
}

} // namespace manimart::filter
