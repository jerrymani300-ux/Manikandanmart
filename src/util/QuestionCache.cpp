#include "util/QuestionCache.hpp"
#include <algorithm>
#include <cctype>

namespace manimart::util {

QuestionCache::QuestionCache(std::chrono::minutes ttl, size_t max_entries)
    : ttl_(ttl), max_entries_(max_entries) {}

std::string QuestionCache::Normalize(const std::string& text) {
    std::string result;
    result.reserve(text.size());
    for (char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        } else if (!result.empty() && result.back() != ' ') {
            result.push_back(' ');
        }
    }
    // Trim trailing space
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    return result;
}

void QuestionCache::Put(const std::string& session_id, const std::string& question, const std::string& answer) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (cache_.size() >= max_entries_) {
        // Evict expired entries or clear half
        auto now = std::chrono::steady_clock::now();
        for (auto it = cache_.begin(); it != cache_.end(); ) {
            if (it->second.expiry < now) {
                it = cache_.erase(it);
            } else {
                ++it;
            }
        }
        if (cache_.size() >= max_entries_) {
            cache_.erase(cache_.begin());
        }
    }

    std::string key = session_id + "::" + Normalize(question);
    cache_[key] = CacheEntry{
        .answer = answer,
        .expiry = std::chrono::steady_clock::now() + ttl_
    };
}

std::optional<std::string> QuestionCache::Get(const std::string& session_id, const std::string& question) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string key = session_id + "::" + Normalize(question);
    auto it = cache_.find(key);
    if (it == cache_.end()) {
        return std::nullopt;
    }

    if (it->second.expiry < std::chrono::steady_clock::now()) {
        cache_.erase(it);
        return std::nullopt;
    }

    return it->second.answer;
}

void QuestionCache::Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    cache_.clear();
}

} // namespace manimart::util
