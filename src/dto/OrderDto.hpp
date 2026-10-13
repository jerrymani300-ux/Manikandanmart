#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "model/Order.hpp"

namespace manimart::dto {

struct CheckoutRequest {
    std::string shipping_address;
    std::string payment_method{"MOCK_CARD"}; // e.g. MOCK_CARD, MOCK_UPI, MOCK_COD
};

inline void from_json(const nlohmann::json& j, CheckoutRequest& req) {
    req.shipping_address = j.value("shipping_address", "");
    req.payment_method = j.value("payment_method", "MOCK_CARD");
}

struct OrderItemResponse {
    int64_t id{0};
    int64_t product_id{0};
    std::string product_name;
    std::string product_image;
    int quantity{1};
    int64_t unit_price_cents{0};
    std::string formatted_unit_price;
    int64_t subtotal_cents{0};
    std::string formatted_subtotal;
};

inline void to_json(nlohmann::json& j, const OrderItemResponse& res) {
    j = {
        {"id", res.id},
        {"product_id", res.product_id},
        {"product_name", res.product_name},
        {"product_image", res.product_image},
        {"quantity", res.quantity},
        {"unit_price_cents", res.unit_price_cents},
        {"formatted_unit_price", res.formatted_unit_price},
        {"subtotal_cents", res.subtotal_cents},
        {"formatted_subtotal", res.formatted_subtotal}
    };
}

struct OrderResponse {
    int64_t id{0};
    int64_t buyer_id{0};
    std::string buyer_name;
    std::string buyer_email;
    std::string status;
    int64_t total_amount_cents{0};
    std::string formatted_total;
    std::string created_at;
    std::vector<OrderItemResponse> items;
};

inline void to_json(nlohmann::json& j, const OrderResponse& res) {
    j = {
        {"id", res.id},
        {"buyer_id", res.buyer_id},
        {"buyer_name", res.buyer_name},
        {"buyer_email", res.buyer_email},
        {"status", res.status},
        {"total_amount_cents", res.total_amount_cents},
        {"formatted_total", res.formatted_total},
        {"created_at", res.created_at},
        {"items", res.items}
    };
}

// Builder pattern for OrderResponse
class OrderDtoBuilder {
public:
    OrderDtoBuilder& FromModel(const model::Order& order) {
        dto_.id = order.id;
        dto_.buyer_id = order.buyer_id;
        dto_.buyer_name = order.buyer_name;
        dto_.buyer_email = order.buyer_email;
        dto_.status = model::OrderStatusToString(order.status);
        dto_.total_amount_cents = order.total_amount.GetCents();
        dto_.formatted_total = order.total_amount.ToFormattedString();
        dto_.created_at = order.created_at;

        dto_.items.clear();
        for (const auto& item : order.items) {
            OrderItemResponse item_dto{
                .id = item.id,
                .product_id = item.product_id,
                .product_name = item.product_name,
                .product_image = item.product_image,
                .quantity = item.quantity,
                .unit_price_cents = item.unit_price.GetCents(),
                .formatted_unit_price = item.unit_price.ToFormattedString(),
                .subtotal_cents = item.GetSubtotal().GetCents(),
                .formatted_subtotal = item.GetSubtotal().ToFormattedString()
            };
            dto_.items.push_back(item_dto);
        }
        return *this;
    }

    OrderResponse Build() const {
        return dto_;
    }

private:
    OrderResponse dto_;
};

} // namespace manimart::dto
