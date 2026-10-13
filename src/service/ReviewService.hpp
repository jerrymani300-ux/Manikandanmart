#pragma once

#include <memory>
#include <vector>
#include "repository/IReviewRepository.hpp"
#include "repository/IOrderRepository.hpp"
#include "repository/IProductRepository.hpp"
#include "dto/ReviewDto.hpp"

namespace manimart::service {

class ReviewService {
public:
    ReviewService(
        std::shared_ptr<repository::IReviewRepository> review_repo,
        std::shared_ptr<repository::IOrderRepository> order_repo,
        std::shared_ptr<repository::IProductRepository> product_repo
    );

    dto::ProductReviewSummaryResponse GetProductReviews(int64_t product_id);
    dto::ReviewResponse AddReview(int64_t user_id, int64_t product_id, const dto::CreateReviewRequest& req);
    bool DeleteReview(int64_t user_id, int64_t review_id, bool is_admin = false);

private:
    std::shared_ptr<repository::IReviewRepository> review_repo_;
    std::shared_ptr<repository::IOrderRepository> order_repo_;
    std::shared_ptr<repository::IProductRepository> product_repo_;
};

} // namespace manimart::service
