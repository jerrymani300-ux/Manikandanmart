#pragma once

#include <string>
#include <optional>
#include <nlohmann/json.hpp>
#include "exception/AppException.hpp"

namespace manimart::dto {

class EnvelopeDto {
public:
    static nlohmann::json Success(const nlohmann::json& data) {
        return {
            {"success", true},
            {"data", data},
            {"error", nullptr}
        };
    }

    static nlohmann::json Success() {
        return {
            {"success", true},
            {"data", {{"message", "Operation completed successfully"}}},
            {"error", nullptr}
        };
    }

    static nlohmann::json Error(const std::string& code, const std::string& message, const std::string& field = "") {
        nlohmann::json err_obj = {
            {"code", code},
            {"message", message}
        };
        if (!field.empty()) {
            err_obj["field"] = field;
        } else {
            err_obj["field"] = nullptr;
        }

        return {
            {"success", false},
            {"data", nullptr},
            {"error", err_obj}
        };
    }

    static nlohmann::json FromException(const exception::AppException& ex) {
        return Error(ex.GetCodeString(), ex.GetMessage(), ex.GetField());
    }
};

} // namespace manimart::dto
