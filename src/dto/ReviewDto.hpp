#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "model/Review.hpp"

namespace manimart::dto {

struct CreateReviewRequest {
    int rating{5};
    std::string comment;
};

inline void from_json(const nlohmann::json& j, CreateReviewRequest& req) {
    req.rating = j.value("rating", 5);
    req.comment = j.value("comment", "");
}

struct ReviewResponse {
    int64_t id{0};
    int64_t product_id{0};
    int64_t user_id{0};
    std::string user_name;
    int rating{5};
    std::string comment;
    std::string created_at;
};

inline void to_json(nlohmann::json& j, const ReviewResponse& res) {
    j = {
        {"id", res.id},
        {"product_id", res.product_id},
        {"user_id", res.user_id},
        {"user_name", res.user_name},
        {"rating", res.rating},
        {"comment", res.comment},
        {"created_at", res.created_at}
    };
}

struct ProductReviewSummaryResponse {
    double average_rating{0.0};
    int total_reviews{0};
    std::vector<ReviewResponse> reviews;
};

inline void to_json(nlohmann::json& j, const ProductReviewSummaryResponse& res) {
    j = {
        {"average_rating", res.average_rating},
        {"total_reviews", res.total_reviews},
        {"reviews", res.reviews}
    };
}

} // namespace manimart::dto
