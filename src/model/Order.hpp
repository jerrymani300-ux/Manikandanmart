#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "model/OrderStatus.hpp"
#include "model/OrderItem.hpp"
#include "util/Money.hpp"

namespace manimart::model {

struct Order {
    int64_t id{0};
    int64_t buyer_id{0};
    std::string buyer_name;
    std::string buyer_email;
    OrderStatus status{OrderStatus::PENDING};
    util::Money total_amount{util::Money::FromCents(0)};
    std::string created_at;
    std::vector<OrderItem> items;
};

} // namespace manimart::model
