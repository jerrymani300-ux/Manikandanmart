#include "provider/MockPaymentStrategy.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <random>

namespace manimart::provider {

PaymentResult MockPaymentStrategy::ProcessPayment(
    int64_t buyer_id,
    const util::Money& amount,
    const std::string& payment_method,
    const std::string& /*billing_details*/) {

    // Validate amount
    if (amount.IsZero() || amount.IsNegative()) {
        return PaymentResult{
            .success = false,
            .transaction_id = "",
            .message = "Invalid payment amount"
        };
    }

    // Generate unique transaction reference
    auto now = std::chrono::system_clock::now().time_since_epoch().count();
    std::ostringstream oss;
    oss << "TXN-" << buyer_id << "-" << (now % 100000000);

    return PaymentResult{
        .success = true,
        .transaction_id = oss.str(),
        .message = "Payment of " + amount.ToFormattedString() + " confirmed via " + payment_method
    };
}

} // namespace manimart::provider
