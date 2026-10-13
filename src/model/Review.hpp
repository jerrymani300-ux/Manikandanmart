#pragma once

#include <cstdint>
#include <string>

namespace manimart::model {

struct Review {
    int64_t id{0};
    int64_t product_id{0};
    int64_t user_id{0};
    std::string user_name;
    int rating{5};
    std::string comment;
    std::string created_at;
};

} // namespace manimart::model
