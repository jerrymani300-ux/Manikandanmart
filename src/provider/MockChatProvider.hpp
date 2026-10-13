#pragma once

#include "provider/ChatProvider.hpp"

namespace manimart::provider {

class MockChatProvider : public ChatProvider {
public:
    std::string GetReply(const std::string& user_message, const std::string& context = "") override;
    [[nodiscard]] std::string GetProviderName() const override { return "MockProvider"; }
};

} // namespace manimart::provider
