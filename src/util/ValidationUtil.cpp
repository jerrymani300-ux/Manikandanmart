#include "util/ValidationUtil.hpp"
#include "util/PasswordHasher.hpp"
#include "exception/AppException.hpp"
#include <algorithm>
#include <cctype>

namespace manimart::util {

static const std::vector<std::string> ALLOWED_CATEGORIES = {
    "Electronics",
    "Fashion",
    "Books",
    "Home",
    "Fitness",
    "Accessories"
};

const std::vector<std::string>& ValidationUtil::GetAllowedCategories() {
    return ALLOWED_CATEGORIES;
}

std::string ValidationUtil::Trim(const std::string& str) {
    auto start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    auto end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

bool ValidationUtil::IsValidEmail(const std::string& email) {
    if (email.length() < 3 || email.length() > 255) return false;
    static const std::regex email_pattern(
        R"(^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)+$)"
    );
    return std::regex_match(email, email_pattern);
}

bool ValidationUtil::IsValidUrl(const std::string& url) {
    if (url.empty() || url.length() > 1024) return false;
    return (url.rfind("http://", 0) == 0 || url.rfind("https://", 0) == 0 || url.rfind("/", 0) == 0);
}

bool ValidationUtil::IsValidCategory(const std::string& category) {
    return std::find(ALLOWED_CATEGORIES.begin(), ALLOWED_CATEGORIES.end(), category) != ALLOWED_CATEGORIES.end();
}

void ValidationUtil::ValidateRegistration(const std::string& name, const std::string& email, const std::string& password, const std::string& role) {
    std::string trimmed_name = Trim(name);
    if (trimmed_name.length() < 2 || trimmed_name.length() > 100) {
        throw exception::ValidationException("Name must be between 2 and 100 characters", "name");
    }

    std::string trimmed_email = Trim(email);
    if (!IsValidEmail(trimmed_email)) {
        throw exception::ValidationException("Invalid email format", "email");
    }

    std::string reason;
    if (!PasswordHasher::ValidatePasswordStrength(password, reason)) {
        throw exception::ValidationException(reason, "password");
    }

    if (role != "BUYER" && role != "SELLER") {
        throw exception::ValidationException("Role must be either BUYER or SELLER (Admin accounts cannot be registered publicly)", "role");
    }
}

void ValidationUtil::ValidateLogin(const std::string& email, const std::string& password) {
    std::string trimmed_email = Trim(email);
    if (trimmed_email.empty() || !IsValidEmail(trimmed_email)) {
        throw exception::ValidationException("Valid email address is required", "email");
    }
    if (password.empty()) {
        throw exception::ValidationException("Password is required", "password");
    }
}

void ValidationUtil::ValidateProduct(const std::string& name, const std::string& description, int64_t price_cents, int stock_qty, const std::string& category, const std::string& image_url) {
    std::string trimmed_name = Trim(name);
    if (trimmed_name.length() < 2 || trimmed_name.length() > 255) {
        throw exception::ValidationException("Product name must be between 2 and 255 characters", "name");
    }

    std::string trimmed_desc = Trim(description);
    if (trimmed_desc.length() < 5 || trimmed_desc.length() > 5000) {
        throw exception::ValidationException("Product description must be between 5 and 5000 characters", "description");
    }

    if (price_cents <= 0) {
        throw exception::ValidationException("Price must be greater than zero", "price");
    }

    if (stock_qty < 0) {
        throw exception::ValidationException("Stock quantity cannot be negative", "stock");
    }

    if (!IsValidCategory(category)) {
        throw exception::ValidationException("Invalid category selected. Must be one of: Electronics, Fashion, Books, Home, Fitness, Accessories", "category");
    }

    if (!IsValidUrl(image_url)) {
        throw exception::ValidationException("Invalid image URL. Must start with http://, https:// or /", "image_url");
    }
}

void ValidationUtil::ValidateReview(int rating, const std::string& comment) {
    if (rating < 1 || rating > 5) {
        throw exception::ValidationException("Rating must be an integer between 1 and 5", "rating");
    }
    std::string trimmed_comment = Trim(comment);
    if (trimmed_comment.length() < 3 || trimmed_comment.length() > 1000) {
        throw exception::ValidationException("Review comment must be between 3 and 1000 characters", "comment");
    }
}

void ValidationUtil::ValidateChatMessage(const std::string& message) {
    std::string trimmed = Trim(message);
    if (trimmed.empty()) {
        throw exception::ValidationException("Chat message cannot be empty", "message");
    }
    if (trimmed.length() > 1000) {
        throw exception::ValidationException("Chat message exceeds maximum allowed length of 1000 characters", "message");
    }
}

std::string ValidationUtil::SanitizeHtml(const std::string& input) {
    std::string output;
    output.reserve(input.size() * 11 / 10);
    for (char c : input) {
        switch (c) {
            case '&': output.append("&amp;"); break;
            case '<': output.append("&lt;"); break;
            case '>': output.append("&gt;"); break;
            case '"': output.append("&quot;"); break;
            case '\'': output.append("&#x27;"); break;
            default: output.push_back(c); break;
        }
    }
    return output;
}

} // namespace manimart::util
