#pragma once

#include "provider/ChatProvider.hpp"
#include "provider/MockChatProvider.hpp"
#include <string>
#include <memory>

namespace manimart::provider {

class GeminiChatProvider : public ChatProvider {
public:
    explicit GeminiChatProvider(std::string api_key = "", std::string model_name = "gemini-1.5-flash", int timeout_ms = 5000);

    std::string GetReply(const std::string& user_message, const std::string& context = "") override;
    [[nodiscard]] std::string GetProviderName() const override { return "GeminiProvider"; }

private:
    std::string api_key_;
    std::string model_name_;
    int timeout_ms_;
    MockChatProvider fallback_provider_;
};

} // namespace manimart::provider
