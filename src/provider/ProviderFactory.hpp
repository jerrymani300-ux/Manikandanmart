#pragma once

#include <memory>
#include <string>
#include "provider/ChatProvider.hpp"
#include "provider/PaymentStrategy.hpp"

namespace manimart::provider {

class ProviderFactory {
public:
    static std::shared_ptr<ChatProvider> CreateChatProvider(const std::string& provider_type = "");
    static std::shared_ptr<IPaymentStrategy> CreatePaymentStrategy(const std::string& strategy_type = "mock");
};

} // namespace manimart::provider
