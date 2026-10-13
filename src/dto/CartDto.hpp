#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "model/CartItem.hpp"
#include "util/Money.hpp"

namespace manimart::dto {

struct AddToCartRequest {
    int64_t product_id{0};
    int quantity{1};
};

inline void from_json(const nlohmann::json& j, AddToCartRequest& req) {
    req.product_id = j.value("product_id", j.value("productId", int64_t{0}));
    req.quantity = j.value("quantity", 1);
}

struct UpdateCartItemRequest {
    int quantity{1};
};

inline void from_json(const nlohmann::json& j, UpdateCartItemRequest& req) {
    req.quantity = j.value("quantity", 1);
}

struct CartItemResponse {
    int64_t id{0};
    int64_t product_id{0};
    std::string product_name;
    std::string product_category;
    std::string product_image_url;
    int64_t unit_price_cents{0};
    std::string formatted_unit_price;
    int product_stock{0};
    int quantity{1};
    int64_t subtotal_cents{0};
    std::string formatted_subtotal;
};

inline void to_json(nlohmann::json& j, const CartItemResponse& res) {
    j = {
        {"id", res.id},
        {"product_id", res.product_id},
        {"product_name", res.product_name},
        {"product_category", res.product_category},
        {"product_image_url", res.product_image_url},
        {"unit_price_cents", res.unit_price_cents},
        {"formatted_unit_price", res.formatted_unit_price},
        {"product_stock", res.product_stock},
        {"quantity", res.quantity},
        {"subtotal_cents", res.subtotal_cents},
        {"formatted_subtotal", res.formatted_subtotal}
    };
}

struct CartResponse {
    std::vector<CartItemResponse> items;
    int total_items{0};
    int64_t total_amount_cents{0};
    std::string formatted_total;
};

inline void to_json(nlohmann::json& j, const CartResponse& res) {
    j = {
        {"items", res.items},
        {"total_items", res.total_items},
        {"total_amount_cents", res.total_amount_cents},
        {"formatted_total", res.formatted_total}
    };
}

} // namespace manimart::dto
