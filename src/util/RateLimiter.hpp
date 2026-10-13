#pragma once

#include <string>
#include <unordered_map>
#include <deque>
#include <chrono>
#include <mutex>

namespace manimart::util {

class RateLimiter {
public:
    explicit RateLimiter(int max_requests_per_minute = 10, std::chrono::seconds window = std::chrono::seconds(60));

    // Returns true if the request is allowed, false if rate limited
    bool AllowRequest(const std::string& key);

    // Resets limit for a key
    void Reset(const std::string& key);

    // Cleans up expired entries periodically
    void Cleanup();

private:
    int max_requests_;
    std::chrono::seconds window_;
    std::mutex mutex_;
    std::unordered_map<std::string, std::deque<std::chrono::steady_clock::time_point>> requests_;
};

} // namespace manimart::util
