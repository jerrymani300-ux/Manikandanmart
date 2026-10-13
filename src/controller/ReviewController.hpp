#pragma once

#include <drogon/HttpController.h>
#include <memory>
#include "service/ReviewService.hpp"

namespace manimart::controller {

class ReviewController : public drogon::HttpController<ReviewController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ReviewController::GetReviews, "/api/v1/products/{1}/reviews", drogon::Get, "manimart::filter::LoggingFilter", "manimart::filter::CorsFilter");
    ADD_METHOD_TO(ReviewController::AddReview, "/api/v1/products/{1}/reviews", drogon::Post, "manimart::filter::LoggingFilter", "manimart::filter::AuthFilter", "manimart::filter::CorsFilter");
    METHOD_LIST_END

    void GetReviews(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t product_id);
    void AddReview(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t product_id);

    static void SetReviewService(std::shared_ptr<service::ReviewService> service);
    static std::shared_ptr<service::ReviewService> GetReviewService();

private:
    static std::shared_ptr<service::ReviewService> review_service_;
};

} // namespace manimart::controller
