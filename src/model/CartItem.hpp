#pragma once

#include <cstdint>
#include <string>
#include "util/Money.hpp"

namespace manimart::model {

struct CartItem {
    int64_t id{0};
    int64_t user_id{0};
    int64_t product_id{0};
    std::string product_name;
    std::string product_category;
    std::string product_image_url;
    util::Money product_price{util::Money::FromCents(0)};
    int product_stock{0};
    int quantity{1};
    std::string created_at;

    [[nodiscard]] util::Money GetSubtotal() const {
        return product_price * quantity;
    }
};

} // namespace manimart::model
