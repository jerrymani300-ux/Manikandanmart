#pragma once

#ifdef SendMessage
    #undef SendMessage
#endif

#include <drogon/HttpController.h>
#include <memory>
#include "service/ChatService.hpp"

namespace manimart::controller {

class ChatController : public drogon::HttpController<ChatController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ChatController::SendMessage, "/api/v1/chat", drogon::Post, "manimart::filter::LoggingFilter", "manimart::filter::CorsFilter");
    METHOD_LIST_END

    void SendMessage(const drogon::HttpRequestPtr& req, std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    static void SetChatService(std::shared_ptr<service::ChatService> service);
    static std::shared_ptr<service::ChatService> GetChatService();

private:
    static std::shared_ptr<service::ChatService> chat_service_;
};

} // namespace manimart::controller
