#include "service/ChatService.hpp"
#include "repository/DatabaseConnection.hpp"
#include "util/ValidationUtil.hpp"
#include "exception/AppException.hpp"
#include <spdlog/spdlog.h>

namespace manimart::service {

ChatService::ChatService(
    std::shared_ptr<provider::ChatProvider> chat_provider,
    std::shared_ptr<util::RateLimiter> rate_limiter,
    std::shared_ptr<util::QuestionCache> question_cache)
    : chat_provider_(std::move(chat_provider)),
      rate_limiter_(std::move(rate_limiter)),
      question_cache_(std::move(question_cache)) {
    if (!chat_provider_ || !rate_limiter_ || !question_cache_) {
        throw exception::AppException(exception::ErrorCode::INTERNAL_SERVER_ERROR, "ChatService: dependencies cannot be null");
    }
}

dto::ChatMessageResponse ChatService::ProcessMessage(
    const std::string& session_id,
    const dto::ChatMessageRequest& req,
    const std::string& marketplace_context) {

    // 1. Validation
    util::ValidationUtil::ValidateChatMessage(req.message);

    std::string clean_message = util::ValidationUtil::Trim(req.message);
    std::string active_session = session_id.empty() ? "anonymous_session" : session_id;

    // 2. Rate limiting (10 msg / min per session)
    if (!rate_limiter_->AllowRequest(active_session)) {
        spdlog::warn("Rate limit triggered for chatbot session: {}", active_session);
        throw exception::RateLimitException("Too many messages sent. Please wait a moment before asking another question.");
    }

    // 3. Check duplicate question cache
    auto cached_answer = question_cache_->Get(active_session, clean_message);
    if (cached_answer.has_value()) {
        spdlog::debug("Serving cached chatbot answer for session: {}", active_session);
        return dto::ChatMessageResponse{
            .reply = *cached_answer,
            .provider = chat_provider_->GetProviderName(),
            .cached = true
        };
    }

    // 4. Query provider
    std::string reply = chat_provider_->GetReply(clean_message, marketplace_context);

    // 5. Cache result
    question_cache_->Put(active_session, clean_message, reply);

    // 6. Audit log in chat_logs table (safe background-style try-catch)
    try {
        const std::string log_sql = "INSERT INTO chat_logs (session_id, user_message, bot_response, provider) VALUES (?, ?, ?, ?);";
        repository::DatabaseConnection::GetInstance().ExecuteUpdate(log_sql, {
            active_session,
            clean_message,
            reply,
            chat_provider_->GetProviderName()
        });
    } catch (...) {
        spdlog::warn("Failed to write audit log for chat session: {}", active_session);
    }

    return dto::ChatMessageResponse{
        .reply = reply,
        .provider = chat_provider_->GetProviderName(),
        .cached = false
    };
}

} // namespace manimart::service
