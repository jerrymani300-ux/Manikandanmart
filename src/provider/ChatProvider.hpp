#pragma once

#include <string>

namespace manimart::provider {

class ChatProvider {
public:
    virtual ~ChatProvider() = default;

    virtual std::string GetReply(
        const std::string& user_message,
        const std::string& context = ""
    ) = 0;

    [[nodiscard]] virtual std::string GetProviderName() const = 0;
};

} // namespace manimart::provider
