#pragma once

#include "provider/PaymentStrategy.hpp"

namespace manimart::provider {

class MockPaymentStrategy : public IPaymentStrategy {
public:
    PaymentResult ProcessPayment(
        int64_t buyer_id,
        const util::Money& amount,
        const std::string& payment_method,
        const std::string& billing_details = ""
    ) override;

    [[nodiscard]] std::string GetStrategyName() const override { return "MockPaymentGateway"; }
};

} // namespace manimart::provider
