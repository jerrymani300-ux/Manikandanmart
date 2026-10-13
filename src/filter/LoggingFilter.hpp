#pragma once

#include <drogon/HttpFilter.h>
#include <string>

namespace manimart::filter {

class LoggingFilter : public drogon::HttpFilter<LoggingFilter> {
public:
    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb
    ) override;

    static std::string GenerateRequestId();
};

} // namespace manimart::filter
