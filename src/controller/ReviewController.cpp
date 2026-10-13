#include "controller/ReviewController.hpp"
#include "dto/EnvelopeDto.hpp"
#include "exception/AppException.hpp"
#include <drogon/HttpResponse.h>
#include <spdlog/spdlog.h>

namespace manimart::controller {

std::shared_ptr<service::ReviewService> ReviewController::review_service_ = nullptr;

void ReviewController::SetReviewService(std::shared_ptr<service::ReviewService> service) {
    review_service_ = std::move(service);
}

std::shared_ptr<service::ReviewService> ReviewController::GetReviewService() {
    return review_service_;
}

void ReviewController::GetReviews(const drogon::HttpRequestPtr& /*req*/, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t product_id) {
    try {
        if (!review_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "ReviewService not initialized");
        }

        auto summary = review_service_->GetProductReviews(product_id);
        nlohmann::json json_res;
        to_json(json_res, summary);

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(json_res).dump());
        resp->setStatusCode(drogon::k200OK);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } catch (const exception::AppException& ex) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::FromException(ex).dump());
        resp->setStatusCode(static_cast<drogon::HttpStatusCode>(ex.GetHttpStatus()));
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    }
}

void ReviewController::AddReview(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback, int64_t product_id) {
    try {
        if (!review_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "ReviewService not initialized");
        }

        auto session = req->session();
        int64_t user_id = session->get<int64_t>("user_id");

        auto json = nlohmann::json::parse(req->body());
        dto::CreateReviewRequest review_req = json.get<dto::CreateReviewRequest>();

        auto review_res = review_service_->AddReview(user_id, product_id, review_req);
        nlohmann::json json_res;
        to_json(json_res, review_res);

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(json_res).dump());
        resp->setStatusCode(drogon::k201Created);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } catch (const exception::AppException& ex) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::FromException(ex).dump());
        resp->setStatusCode(static_cast<drogon::HttpStatusCode>(ex.GetHttpStatus()));
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } catch (const std::exception& ex) {
        spdlog::error("Error in AddReview: {}", ex.what());
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            dto::EnvelopeDto::Error("INTERNAL_SERVER_ERROR", "Failed to submit review").dump()
        );
        resp->setStatusCode(drogon::k500InternalServerError);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    }
}

} // namespace manimart::controller
