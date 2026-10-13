#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace manimart::model {

enum class OrderStatus {
    PENDING,
    CONFIRMED,
    SHIPPED,
    DELIVERED,
    CANCELLED
};

inline std::string OrderStatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::PENDING: return "PENDING";
        case OrderStatus::CONFIRMED: return "CONFIRMED";
        case OrderStatus::SHIPPED: return "SHIPPED";
        case OrderStatus::DELIVERED: return "DELIVERED";
        case OrderStatus::CANCELLED: return "CANCELLED";
    }
    return "PENDING";
}

inline OrderStatus StringToOrderStatus(const std::string& str) {
    if (str == "CONFIRMED") return OrderStatus::CONFIRMED;
    if (str == "SHIPPED") return OrderStatus::SHIPPED;
    if (str == "DELIVERED") return OrderStatus::DELIVERED;
    if (str == "CANCELLED") return OrderStatus::CANCELLED;
    return OrderStatus::PENDING;
}

inline bool CanTransition(OrderStatus from, OrderStatus to) {
    switch (from) {
        case OrderStatus::PENDING:
            return (to == OrderStatus::CONFIRMED || to == OrderStatus::CANCELLED);
        case OrderStatus::CONFIRMED:
            return (to == OrderStatus::SHIPPED || to == OrderStatus::CANCELLED);
        case OrderStatus::SHIPPED:
            return (to == OrderStatus::DELIVERED);
        case OrderStatus::DELIVERED:
        case OrderStatus::CANCELLED:
            return false;
    }
    return false;
}

inline void to_json(nlohmann::json& j, const OrderStatus& s) {
    j = OrderStatusToString(s);
}

inline void from_json(const nlohmann::json& j, OrderStatus& s) {
    s = StringToOrderStatus(j.get<std::string>());
}

} // namespace manimart::model
