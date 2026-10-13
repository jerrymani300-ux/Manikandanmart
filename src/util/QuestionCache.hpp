#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <chrono>
#include <mutex>

namespace manimart::util {

class QuestionCache {
public:
    explicit QuestionCache(std::chrono::minutes ttl = std::chrono::minutes(30), size_t max_entries = 500);

    void Put(const std::string& session_id, const std::string& question, const std::string& answer);
    [[nodiscard]] std::optional<std::string> Get(const std::string& session_id, const std::string& question);
    void Clear();

private:
    static std::string Normalize(const std::string& text);

    struct CacheEntry {
        std::string answer;
        std::chrono::steady_clock::time_point expiry;
    };

    std::chrono::minutes ttl_;
    size_t max_entries_;
    std::mutex mutex_;
    // Map key: session_id + "::" + normalized_question
    std::unordered_map<std::string, CacheEntry> cache_;
};

} // namespace manimart::util
