#include "service/ReviewService.hpp"
#include "util/ValidationUtil.hpp"
#include "exception/AppException.hpp"
#include <spdlog/spdlog.h>

namespace manimart::service {

ReviewService::ReviewService(
    std::shared_ptr<repository::IReviewRepository> review_repo,
    std::shared_ptr<repository::IOrderRepository> order_repo,
    std::shared_ptr<repository::IProductRepository> product_repo)
    : review_repo_(std::move(review_repo)),
      order_repo_(std::move(order_repo)),
      product_repo_(std::move(product_repo)) {
    if (!review_repo_ || !order_repo_ || !product_repo_) {
        throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "ReviewService: repositories cannot be null");
    }
}

dto::ProductReviewSummaryResponse ReviewService::GetProductReviews(int64_t product_id) {
    auto reviews = review_repo_->FindByProductId(product_id);
    double avg_rating = review_repo_->GetAverageRating(product_id);
    int count = review_repo_->GetReviewCount(product_id);

    dto::ProductReviewSummaryResponse summary;
    summary.average_rating = avg_rating;
    summary.total_reviews = count;
    summary.reviews.reserve(reviews.size());

    for (const auto& rev : reviews) {
        dto::ReviewResponse res{
            .id = rev.id,
            .product_id = rev.product_id,
            .user_id = rev.user_id,
            .user_name = rev.user_name,
            .rating = rev.rating,
            .comment = rev.comment,
            .created_at = rev.created_at
        };
        summary.reviews.push_back(res);
    }
    return summary;
}

dto::ReviewResponse ReviewService::AddReview(int64_t user_id, int64_t product_id, const dto::CreateReviewRequest& req) {
    // 1. Verify Product exists
    auto product = product_repo_->FindById(product_id);
    if (!product.has_value()) {
        throw exception::NotFoundException("Product not found", "product_id");
    }

    // 2. Validate input
    util::ValidationUtil::ValidateReview(req.rating, req.comment);

    // 3. Verify user has actually purchased the product in a DELIVERED order
    bool purchased = order_repo_->HasUserPurchasedProduct(user_id, product_id);
    if (!purchased) {
        throw exception::AuthorizationException("Only verified buyers who have received a delivered order for this product can submit a review.");
    }

    // 4. Prevent duplicate review for the same product by the same user
    auto existing_rev = review_repo_->FindByProductAndUser(product_id, user_id);
    if (existing_rev.has_value()) {
        throw exception::ConflictException("You have already reviewed this product.");
    }

    model::Review review{
        .product_id = product_id,
        .user_id = user_id,
        .rating = req.rating,
        .comment = util::ValidationUtil::Trim(req.comment)
    };

    int64_t id = review_repo_->Create(review);
    if (id <= 0) {
        throw exception::DatabaseException("Failed to save review");
    }

    review.id = id;
    spdlog::info("Review {} submitted by user {} on product {}", id, user_id, product_id);

    return dto::ReviewResponse{
        .id = review.id,
        .product_id = review.product_id,
        .user_id = review.user_id,
        .user_name = "You",
        .rating = review.rating,
        .comment = review.comment,
        .created_at = "Just now"
    };
}

bool ReviewService::DeleteReview(int64_t user_id, int64_t review_id, bool is_admin) {
    if (!is_admin) {
        // Only admin can delete arbitrary reviews
        throw exception::AuthorizationException("Only administrators can moderate reviews");
    }
    return review_repo_->Delete(review_id);
}

} // namespace manimart::service
