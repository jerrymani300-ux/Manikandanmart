#pragma once

#include <string>
#include "util/Money.hpp"

namespace manimart::provider {

struct PaymentResult {
    bool success{false};
    std::string transaction_id;
    std::string message;
};

class IPaymentStrategy {
public:
    virtual ~IPaymentStrategy() = default;

    virtual PaymentResult ProcessPayment(
        int64_t buyer_id,
        const util::Money& amount,
        const std::string& payment_method,
        const std::string& billing_details = ""
    ) = 0;

    [[nodiscard]] virtual std::string GetStrategyName() const = 0;
};

} // namespace manimart::provider
