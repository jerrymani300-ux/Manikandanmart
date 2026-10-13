#pragma once

#include "repository/IReviewRepository.hpp"
#include "repository/DatabaseConnection.hpp"

namespace manimart::repository {

class SqlReviewRepository : public IReviewRepository {
public:
    std::vector<model::Review> FindByProductId(int64_t product_id) override;
    std::optional<model::Review> FindByProductAndUser(int64_t product_id, int64_t user_id) override;
    int64_t Create(const model::Review& review) override;
    bool Delete(int64_t id) override;
    double GetAverageRating(int64_t product_id) override;
    int GetReviewCount(int64_t product_id) override;

private:
    static model::Review MapRow(const SqlRow& row);
};

} // namespace manimart::repository
