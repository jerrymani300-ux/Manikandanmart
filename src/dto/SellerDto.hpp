#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "dto/ProductDto.hpp"
#include "dto/OrderDto.hpp"

namespace manimart::dto {

struct SellerDashboardStatsResponse {
    int total_products{0};
    int total_orders{0};
    int64_t total_revenue_cents{0};
    std::string formatted_revenue;
    int pending_orders{0};
    std::vector<ProductResponse> top_products;
    std::vector<OrderResponse> recent_orders;
};

inline void to_json(nlohmann::json& j, const SellerDashboardStatsResponse& res) {
    j = {
        {"total_products", res.total_products},
        {"total_orders", res.total_orders},
        {"total_revenue_cents", res.total_revenue_cents},
        {"formatted_revenue", res.formatted_revenue},
        {"pending_orders", res.pending_orders},
        {"top_products", res.top_products},
        {"recent_orders", res.recent_orders}
    };
}

} // namespace manimart::dto
