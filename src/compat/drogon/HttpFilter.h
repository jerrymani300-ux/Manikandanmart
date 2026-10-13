#pragma once

#include "HttpRequest.h"
#include "HttpResponse.h"
#include <functional>

namespace drogon {

using FilterCallback = std::function<void(const HttpResponsePtr&)>;
using FilterChainCallback = std::function<void()>;

template <typename T>
class HttpFilter {
public:
    virtual ~HttpFilter() = default;

    virtual void doFilter(
        const HttpRequestPtr& req,
        FilterCallback&& fcb,
        FilterChainCallback&& fccb
    ) = 0;
};

} // namespace drogon
