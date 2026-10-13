#pragma once

#include <drogon/HttpFilter.h>

namespace manimart::filter {

class SellerFilter : public drogon::HttpFilter<SellerFilter> {
public:
    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb
    ) override;
};

class AdminFilter : public drogon::HttpFilter<AdminFilter> {
public:
    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb
    ) override;
};

} // namespace manimart::filter
