#pragma once

#include <drogon/HttpFilter.h>

namespace manimart::filter {

class CorsFilter : public drogon::HttpFilter<CorsFilter> {
public:
    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb
    ) override;
};

} // namespace manimart::filter
