#include "provider/ProviderFactory.hpp"
#include "provider/MockChatProvider.hpp"
#include "provider/GeminiChatProvider.hpp"
#include "provider/MockPaymentStrategy.hpp"
#include "util/Env.hpp"
#include <algorithm>
#include <cctype>

namespace manimart::provider {

std::shared_ptr<ChatProvider> ProviderFactory::CreateChatProvider(const std::string& provider_type) {
    std::string type = provider_type;
    if (type.empty()) {
        type = util::Env::Get("AI_PROVIDER", "mock");
    }

    std::transform(type.begin(), type.end(), type.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    if (type == "gemini") {
        std::string api_key = util::Env::Get("GEMINI_API_KEY", "");
        std::string model = util::Env::Get("GEMINI_MODEL", "gemini-1.5-flash");
        int timeout = util::Env::GetInt("CHAT_TIMEOUT_MS", 5000);
        return std::make_shared<GeminiChatProvider>(api_key, model, timeout);
    }

    return std::make_shared<MockChatProvider>();
}

std::shared_ptr<IPaymentStrategy> ProviderFactory::CreatePaymentStrategy(const std::string& /*strategy_type*/) {
    return std::make_shared<MockPaymentStrategy>();
}

} // namespace manimart::provider
