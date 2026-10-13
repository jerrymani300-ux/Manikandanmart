#pragma once

#include <memory>
#include <string>
#include "provider/ChatProvider.hpp"
#include "util/RateLimiter.hpp"
#include "util/QuestionCache.hpp"
#include "dto/ChatDto.hpp"

namespace manimart::service {

class ChatService {
public:
    ChatService(
        std::shared_ptr<provider::ChatProvider> chat_provider,
        std::shared_ptr<util::RateLimiter> rate_limiter,
        std::shared_ptr<util::QuestionCache> question_cache
    );

    dto::ChatMessageResponse ProcessMessage(
        const std::string& session_id,
        const dto::ChatMessageRequest& req,
        const std::string& marketplace_context = ""
    );

private:
    std::shared_ptr<provider::ChatProvider> chat_provider_;
    std::shared_ptr<util::RateLimiter> rate_limiter_;
    std::shared_ptr<util::QuestionCache> question_cache_;
};

} // namespace manimart::service
