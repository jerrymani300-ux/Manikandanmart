#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace manimart::dto {

struct ChatMessageRequest {
    std::string message;
};

inline void from_json(const nlohmann::json& j, ChatMessageRequest& req) {
    req.message = j.value("message", "");
}

struct ChatMessageResponse {
    std::string reply;
    std::string provider;
    bool cached{false};
};

inline void to_json(nlohmann::json& j, const ChatMessageResponse& res) {
    j = {
        {"reply", res.reply},
        {"provider", res.provider},
        {"cached", res.cached}
    };
}

} // namespace manimart::dto
