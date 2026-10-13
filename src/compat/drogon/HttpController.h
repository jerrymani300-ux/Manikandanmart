#pragma once

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpFilter.h"

namespace drogon {

#define METHOD_LIST_BEGIN
#define METHOD_LIST_END
#define ADD_METHOD_TO(...)

template <typename T>
class HttpController {
public:
    virtual ~HttpController() = default;
};

} // namespace drogon
