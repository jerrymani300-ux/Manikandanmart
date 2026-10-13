#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "model/Product.hpp"

namespace manimart::dto {

struct CreateProductRequest {
    std::string name;
    std::string description;
    int64_t price_cents{0};
    int stock_qty{0};
    std::string category;
    std::string image_url;
};

inline void from_json(const nlohmann::json& j, CreateProductRequest& req) {
    req.name = j.value("name", "");
    req.description = j.value("description", "");
    if (j.contains("price_cents") && j["price_cents"].is_number()) {
        req.price_cents = j["price_cents"].get<int64_t>();
    } else if (j.contains("price")) {
        if (j["price"].is_number()) {
            // If passed as dollars e.g. 19.99, convert to cents
            double p = j["price"].get<double>();
            req.price_cents = static_cast<int64_t>(std::round(p * 100.0));
        } else if (j["price"].is_string()) {
            req.price_cents = util::Money::FromString(j["price"].get<std::string>()).GetCents();
        }
    }
    req.stock_qty = j.value("stock_qty", j.value("stock", 0));
    req.category = j.value("category", "");
    req.image_url = j.value("image_url", "");
}

struct UpdateProductRequest {
    std::string name;
    std::string description;
    int64_t price_cents{0};
    int stock_qty{0};
    std::string category;
    std::string image_url;
};

inline void from_json(const nlohmann::json& j, UpdateProductRequest& req) {
    req.name = j.value("name", "");
    req.description = j.value("description", "");
    if (j.contains("price_cents") && j["price_cents"].is_number()) {
        req.price_cents = j["price_cents"].get<int64_t>();
    } else if (j.contains("price")) {
        if (j["price"].is_number()) {
            double p = j["price"].get<double>();
            req.price_cents = static_cast<int64_t>(std::round(p * 100.0));
        } else if (j["price"].is_string()) {
            req.price_cents = util::Money::FromString(j["price"].get<std::string>()).GetCents();
        }
    }
    req.stock_qty = j.value("stock_qty", j.value("stock", 0));
    req.category = j.value("category", "");
    req.image_url = j.value("image_url", "");
}

struct ProductResponse {
    int64_t id{0};
    int64_t seller_id{0};
    std::string seller_name;
    std::string name;
    std::string description;
    int64_t price_cents{0};
    std::string formatted_price;
    int stock_qty{0};
    std::string category;
    std::string image_url;
    double average_rating{0.0};
    int review_count{0};
    std::string created_at;
};

inline void to_json(nlohmann::json& j, const ProductResponse& res) {
    j = {
        {"id", res.id},
        {"seller_id", res.seller_id},
        {"seller_name", res.seller_name},
        {"name", res.name},
        {"description", res.description},
        {"price_cents", res.price_cents},
        {"formatted_price", res.formatted_price},
        {"stock_qty", res.stock_qty},
        {"category", res.category},
        {"image_url", res.image_url},
        {"average_rating", res.average_rating},
        {"review_count", res.review_count},
        {"created_at", res.created_at}
    };
}

// Builder pattern for ProductResponse
class ProductDtoBuilder {
public:
    ProductDtoBuilder& FromModel(const model::Product& product) {
        dto_.id = product.id;
        dto_.seller_id = product.seller_id;
        dto_.seller_name = product.seller_name;
        dto_.name = product.name;
        dto_.description = product.description;
        dto_.price_cents = product.price.GetCents();
        dto_.formatted_price = product.price.ToFormattedString();
        dto_.stock_qty = product.stock_qty;
        dto_.category = product.category;
        dto_.image_url = product.image_url;
        dto_.average_rating = product.average_rating;
        dto_.review_count = product.review_count;
        dto_.created_at = product.created_at;
        return *this;
    }

    ProductResponse Build() const {
        return dto_;
    }

private:
    ProductResponse dto_;
};

} // namespace manimart::dto
