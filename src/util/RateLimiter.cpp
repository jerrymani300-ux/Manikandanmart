#include "util/RateLimiter.hpp"

namespace manimart::util {

RateLimiter::RateLimiter(int max_requests_per_minute, std::chrono::seconds window)
    : max_requests_(max_requests_per_minute), window_(window) {}

bool RateLimiter::AllowRequest(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();
    auto cutoff = now - window_;

    auto& timestamps = requests_[key];

    // Remove expired timestamps
    while (!timestamps.empty() && timestamps.front() < cutoff) {
        timestamps.pop_front();
    }

    if (static_cast<int>(timestamps.size()) >= max_requests_) {
        return false;
    }

    timestamps.push_back(now);
    return true;
}

void RateLimiter::Reset(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    requests_.erase(key);
}

void RateLimiter::Cleanup() {
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();
    auto cutoff = now - window_;

    for (auto it = requests_.begin(); it != requests_.end(); ) {
        while (!it->second.empty() && it->second.front() < cutoff) {
            it->second.pop_front();
        }
        if (it->second.empty()) {
            it = requests_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace manimart::util
