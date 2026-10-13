#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "model/Review.hpp"

namespace manimart::repository {

class IReviewRepository {
public:
    virtual ~IReviewRepository() = default;

    virtual std::vector<model::Review> FindByProductId(int64_t product_id) = 0;
    virtual std::optional<model::Review> FindByProductAndUser(int64_t product_id, int64_t user_id) = 0;
    virtual int64_t Create(const model::Review& review) = 0;
    virtual bool Delete(int64_t id) = 0;
    virtual double GetAverageRating(int64_t product_id) = 0;
    virtual int GetReviewCount(int64_t product_id) = 0;
};

} // namespace manimart::repository
