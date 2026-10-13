#pragma once

#include <cstdint>
#include <string>
#include "util/Money.hpp"

namespace manimart::model {

struct OrderItem {
    int64_t id{0};
    int64_t order_id{0};
    int64_t product_id{0};
    std::string product_name;
    std::string product_image;
    int quantity{1};
    util::Money unit_price{util::Money::FromCents(0)};
    std::string created_at;

    [[nodiscard]] util::Money GetSubtotal() const {
        return unit_price * quantity;
    }
};

} // namespace manimart::model
