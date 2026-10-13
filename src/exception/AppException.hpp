#pragma once

#include <exception>
#include <string>

namespace manimart::exception {

enum class ErrorCode {
    VALIDATION_ERROR,
    AUTHENTICATION_ERROR,
    AUTHORIZATION_ERROR,
    NOT_FOUND,
    CONFLICT,
    DATABASE_ERROR,
    RATE_LIMIT_EXCEEDED,
    INTERNAL_SERVER_ERROR
};

inline std::string ErrorCodeToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::VALIDATION_ERROR: return "VALIDATION_ERROR";
        case ErrorCode::AUTHENTICATION_ERROR: return "AUTHENTICATION_ERROR";
        case ErrorCode::AUTHORIZATION_ERROR: return "AUTHORIZATION_ERROR";
        case ErrorCode::NOT_FOUND: return "NOT_FOUND";
        case ErrorCode::CONFLICT: return "CONFLICT";
        case ErrorCode::DATABASE_ERROR: return "DATABASE_ERROR";
        case ErrorCode::RATE_LIMIT_EXCEEDED: return "RATE_LIMIT_EXCEEDED";
        case ErrorCode::INTERNAL_SERVER_ERROR: return "INTERNAL_SERVER_ERROR";
        default: return "UNKNOWN_ERROR";
    }
}

class AppException : public std::exception {
public:
    AppException(ErrorCode code, std::string message, std::string field = "", int http_status = 500)
        : code_(code), message_(std::move(message)), field_(std::move(field)), http_status_(http_status) {}

    [[nodiscard]] const char* what() const noexcept override {
        return message_.c_str();
    }

    [[nodiscard]] ErrorCode GetCode() const noexcept { return code_; }
    [[nodiscard]] const std::string& GetCodeString() const noexcept {
        static const auto str_fn = [](ErrorCode c) { return ErrorCodeToString(c); };
        static std::string last_str;
        last_str = str_fn(code_);
        return last_str;
    }
    [[nodiscard]] const std::string& GetMessage() const noexcept { return message_; }
    [[nodiscard]] const std::string& GetField() const noexcept { return field_; }
    [[nodiscard]] int GetHttpStatus() const noexcept { return http_status_; }

private:
    ErrorCode code_;
    std::string message_;
    std::string field_;
    int http_status_;
};

class ValidationException : public AppException {
public:
    explicit ValidationException(std::string message, std::string field = "")
        : AppException(ErrorCode::VALIDATION_ERROR, std::move(message), std::move(field), 400) {}
};

class AuthenticationException : public AppException {
public:
    explicit AuthenticationException(std::string message = "Invalid email or password")
        : AppException(ErrorCode::AUTHENTICATION_ERROR, std::move(message), "", 401) {}
};

class AuthorizationException : public AppException {
public:
    explicit AuthorizationException(std::string message = "Access denied")
        : AppException(ErrorCode::AUTHORIZATION_ERROR, std::move(message), "", 403) {}
};

class NotFoundException : public AppException {
public:
    explicit NotFoundException(std::string message, std::string field = "")
        : AppException(ErrorCode::NOT_FOUND, std::move(message), std::move(field), 404) {}
};

class ConflictException : public AppException {
public:
    explicit ConflictException(std::string message, std::string field = "")
        : AppException(ErrorCode::CONFLICT, std::move(message), std::move(field), 409) {}
};

class DatabaseException : public AppException {
public:
    explicit DatabaseException(std::string message)
        : AppException(ErrorCode::DATABASE_ERROR, std::move(message), "", 500) {}
};

class RateLimitException : public AppException {
public:
    explicit RateLimitException(std::string message = "Rate limit exceeded. Please wait a moment.")
        : AppException(ErrorCode::RATE_LIMIT_EXCEEDED, std::move(message), "", 429) {}
};

} // namespace manimart::exception
