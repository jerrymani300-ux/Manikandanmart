#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "dto/AuthDto.hpp"
#include "dto/ProductDto.hpp"
#include "dto/OrderDto.hpp"

namespace manimart::dto {

struct AdminDashboardStatsResponse {
    int total_users{0};
    int total_buyers{0};
    int total_sellers{0};
    int total_products{0};
    int total_orders{0};
    int64_t total_gross_sales_cents{0};
    std::string formatted_gross_sales;
    std::vector<UserResponse> recent_users;
    std::vector<OrderResponse> recent_orders;
};

inline void to_json(nlohmann::json& j, const AdminDashboardStatsResponse& res) {
    j = {
        {"total_users", res.total_users},
        {"total_buyers", res.total_buyers},
        {"total_sellers", res.total_sellers},
        {"total_products", res.total_products},
        {"total_orders", res.total_orders},
        {"total_gross_sales_cents", res.total_gross_sales_cents},
        {"formatted_gross_sales", res.formatted_gross_sales},
        {"recent_users", res.recent_users},
        {"recent_orders", res.recent_orders}
    };
}

} // namespace manimart::dto
