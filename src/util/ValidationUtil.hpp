#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <regex>

namespace manimart::util {

class ValidationUtil {
public:
    static bool IsValidEmail(const std::string& email);
    static bool IsValidUrl(const std::string& url);
    static bool IsValidCategory(const std::string& category);
    static const std::vector<std::string>& GetAllowedCategories();

    static void ValidateRegistration(const std::string& name, const std::string& email, const std::string& password, const std::string& role);
    static void ValidateLogin(const std::string& email, const std::string& password);
    static void ValidateProduct(const std::string& name, const std::string& description, int64_t price_cents, int stock_qty, const std::string& category, const std::string& image_url);
    static void ValidateReview(int rating, const std::string& comment);
    static void ValidateChatMessage(const std::string& message);

    static std::string SanitizeHtml(const std::string& input);
    static std::string Trim(const std::string& str);
};

} // namespace manimart::util
