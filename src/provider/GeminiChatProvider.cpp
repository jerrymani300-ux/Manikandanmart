#include "provider/GeminiChatProvider.hpp"
#include "util/Env.hpp"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#if __has_include(<curl/curl.h>)
#include <curl/curl.h>
#define HAVE_CURL 1
#else
#define HAVE_CURL 0
#endif

namespace manimart::provider {

#if HAVE_CURL
static size_t CurlWriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t total_size = size * nmemb;
    userp->append(static_cast<char*>(contents), total_size);
    return total_size;
}
#endif

GeminiChatProvider::GeminiChatProvider(std::string api_key, std::string model_name, int timeout_ms)
    : api_key_(std::move(api_key)), model_name_(std::move(model_name)), timeout_ms_(timeout_ms) {
    if (api_key_.empty()) {
        api_key_ = util::Env::Get("GEMINI_API_KEY", "");
    }
}

std::string GeminiChatProvider::GetReply(const std::string& user_message, const std::string& context) {
    if (api_key_.empty()) {
        spdlog::debug("GEMINI_API_KEY is not configured; using MockChatProvider fallback");
        return fallback_provider_.GetReply(user_message, context);
    }

#if HAVE_CURL
    std::string url = "https://generativelanguage.googleapis.com/v1beta/models/" + model_name_ + ":generateContent?key=" + api_key_;

    std::string system_instruction = 
        "You are the official customer support AI assistant for ManiMart, a multi-seller e-commerce marketplace. "
        "Only answer questions related to ManiMart, product listings, orders, checkout, shopping, and selling on ManiMart. "
        "Be helpful, polite, concise, and professional.";

    if (!context.empty()) {
        system_instruction += "\nCurrent Marketplace Context: " + context;
    }

    nlohmann::json payload = {
        {"contents", nlohmann::json::array({
            {
                {"role", "user"},
                {"parts", nlohmann::json::array({
                    {{"text", system_instruction + "\n\nUser Question: " + user_message}}
                })}
            }
        })},
        {"generationConfig", {
            {"temperature", 0.4},
            {"maxOutputTokens", 300}
        }}
    };

    std::string request_body = payload.dump();
    std::string response_buffer;

    CURL* curl = curl_easy_init();
    if (!curl) {
        spdlog::warn("Failed to initialize cURL, falling back to mock provider");
        return fallback_provider_.GetReply(user_message, context);
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, static_cast<long>(timeout_ms_));
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

    CURLcode res = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK || http_code != 200) {
        spdlog::warn("Gemini API call failed (code: {}, HTTP: {}), falling back to mock provider", static_cast<int>(res), http_code);
        return fallback_provider_.GetReply(user_message, context);
    }

    try {
        auto parsed = nlohmann::json::parse(response_buffer);
        if (parsed.contains("candidates") && !parsed["candidates"].empty()) {
            const auto& candidate = parsed["candidates"][0];
            if (candidate.contains("content") && candidate["content"].contains("parts") && !candidate["content"]["parts"].empty()) {
                return candidate["content"]["parts"][0].value("text", "");
            }
        }
    } catch (...) {
        spdlog::error("Error parsing Gemini API response, using mock provider");
    }

#endif

    return fallback_provider_.GetReply(user_message, context);
}

} // namespace manimart::provider
