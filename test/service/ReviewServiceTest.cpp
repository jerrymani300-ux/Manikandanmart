#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "service/ReviewService.hpp"
#include "test_helpers/MockRepositories.hpp"
#include "exception/AppException.hpp"

using namespace manimart;
using namespace testing;

class ReviewServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_review_repo_ = std::make_shared<test::MockReviewRepository>();
        mock_order_repo_ = std::make_shared<test::MockOrderRepository>();
        mock_product_repo_ = std::make_shared<test::MockProductRepository>();

        review_service_ = std::make_unique<service::ReviewService>(
            mock_review_repo_,
            mock_order_repo_,
            mock_product_repo_
        );
    }

    std::shared_ptr<test::MockReviewRepository> mock_review_repo_;
    std::shared_ptr<test::MockOrderRepository> mock_order_repo_;
    std::shared_ptr<test::MockProductRepository> mock_product_repo_;
    std::unique_ptr<service::ReviewService> review_service_;
};

TEST_F(ReviewServiceTest, AddReviewUnauthorizedIfNotPurchased) {
    model::Product product{.id = 5, .name = "Earphones"};
    EXPECT_CALL(*mock_product_repo_, FindById(5)).WillOnce(Return(product));

    // User has NOT purchased the product in a delivered order
    EXPECT_CALL(*mock_order_repo_, HasUserPurchasedProduct(10, 5)).WillOnce(Return(false));

    dto::CreateReviewRequest req{.rating = 5, .comment = "Great sound quality!"};
    EXPECT_THROW(review_service_->AddReview(10, 5, req), exception::AuthorizationException);
}

TEST_F(ReviewServiceTest, AddReviewInvalidRatingThrowsValidation) {
    model::Product product{.id = 5, .name = "Earphones"};
    EXPECT_CALL(*mock_product_repo_, FindById(5)).WillOnce(Return(product));

    dto::CreateReviewRequest req{.rating = 6, .comment = "Rating out of range 1-5"};
    EXPECT_THROW(review_service_->AddReview(10, 5, req), exception::ValidationException);
}

TEST_F(ReviewServiceTest, AddReviewSuccessForVerifiedBuyer) {
    model::Product product{.id = 5, .name = "Earphones"};
    EXPECT_CALL(*mock_product_repo_, FindById(5)).WillOnce(Return(product));
    EXPECT_CALL(*mock_order_repo_, HasUserPurchasedProduct(10, 5)).WillOnce(Return(true));
    EXPECT_CALL(*mock_review_repo_, FindByProductAndUser(5, 10)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*mock_review_repo_, Create(_)).WillOnce(Return(99));

    dto::CreateReviewRequest req{.rating = 5, .comment = "Amazing build and battery life!"};
    auto res = review_service_->AddReview(10, 5, req);

    EXPECT_EQ(res.id, 99);
    EXPECT_EQ(res.rating, 5);
    EXPECT_EQ(res.comment, "Amazing build and battery life!");
}
