#include "filter/LoggingFilter.hpp"
#include <spdlog/spdlog.h>
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace manimart::filter {

std::string LoggingFilter::GenerateRequestId() {
    static thread_local std::mt19937_64 rng(static_cast<uint64_t>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
    ));
    std::uniform_int_distribution<uint64_t> dist;
    uint64_t r1 = dist(rng);
    uint64_t r2 = dist(rng);

    std::ostringstream oss;
    oss << std::hex << std::setfill('0')
        << std::setw(16) << r1 << "-"
        << std::setw(16) << r2;
    return oss.str();
}

void LoggingFilter::doFilter(
    const drogon::HttpRequestPtr& req,
    drogon::FilterCallback&& /*fcb*/,
    drogon::FilterChainCallback&& fccb) {

    std::string req_id = req->getHeader("X-Request-ID");
    if (req_id.empty()) {
        req_id = GenerateRequestId();
    }

    req->attributes()->insert("request_id", req_id);

    std::string method = req->getMethodString();
    std::string path = req->getPath();
    std::string ip = req->getPeerAddr().toIp();

    spdlog::info("[{}] {} {} from {}", req_id, method, path, ip);

    fccb();
}

} // namespace manimart::filter
