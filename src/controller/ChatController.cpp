#include "controller/ChatController.hpp"
#include "dto/EnvelopeDto.hpp"
#include "exception/AppException.hpp"
#include <drogon/HttpResponse.h>
#include <spdlog/spdlog.h>

namespace manimart::controller {

std::shared_ptr<service::ChatService> ChatController::chat_service_ = nullptr;

void ChatController::SetChatService(std::shared_ptr<service::ChatService> service) {
    chat_service_ = std::move(service);
}

std::shared_ptr<service::ChatService> ChatController::GetChatService() {
    return chat_service_;
}

void ChatController::SendMessage(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!chat_service_) {
            throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "ChatService not initialized");
        }

        std::string session_id = "anon";
        auto session = req->session();
        if (session) {
            session_id = session->sessionId();
        }

        auto json = nlohmann::json::parse(req->body());
        dto::ChatMessageRequest chat_req = json.get<dto::ChatMessageRequest>();

        auto chat_res = chat_service_->ProcessMessage(session_id, chat_req);
        nlohmann::json json_res;
        to_json(json_res, chat_res);

        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::Success(json_res).dump());
        resp->setStatusCode(drogon::k200OK);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } catch (const exception::AppException& ex) {
        auto resp = drogon::HttpResponse::newHttpJsonResponse(dto::EnvelopeDto::FromException(ex).dump());
        resp->setStatusCode(static_cast<drogon::HttpStatusCode>(ex.GetHttpStatus()));
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    } catch (const std::exception& ex) {
        spdlog::error("Error in ChatController: {}", ex.what());
        auto resp = drogon::HttpResponse::newHttpJsonResponse(
            dto::EnvelopeDto::Error("INTERNAL_SERVER_ERROR", "Error communicating with AI Assistant").dump()
        );
        resp->setStatusCode(drogon::k500InternalServerError);
        resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
        callback(resp);
    }
}

} // namespace manimart::controller
