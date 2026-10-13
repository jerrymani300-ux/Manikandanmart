#pragma once

#include <drogon/plugins/PluginBase.h>

namespace manimart::plugin {

class DatabasePlugin : public drogon::PluginBase {
public:
    void initAndStart(const Json::Value& config) override;
    void shutdown() override;
};

} // namespace manimart::plugin
