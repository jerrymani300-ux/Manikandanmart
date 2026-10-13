#pragma once

#include <json/json.h>

namespace drogon {

class PluginBase {
public:
    virtual ~PluginBase() = default;
    virtual void initAndStart(const Json::Value& config) = 0;
    virtual void shutdown() = 0;
};

} // namespace drogon
