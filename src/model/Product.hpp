#pragma once

#include <cstdint>
#include <string>
#include "util/Money.hpp"

namespace manimart::model {

struct Product {
    int64_t id{0};
    int64_t seller_id{0};
    std::string seller_name;
    std::string name;
    std::string description;
    util::Money price{util::Money::FromCents(0)};
    int stock_qty{0};
    std::string category;
    std::string image_url;
    std::string created_at;

    // Aggregates for UI
    double average_rating{0.0};
    int review_count{0};
};

} // namespace manimart::model
