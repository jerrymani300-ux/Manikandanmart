#pragma once

#include <string>

namespace manimart::util {

class Env {
public:
    static void LoadDotEnv(const std::string& filepath = ".env");
    static std::string Get(const std::string& key, const std::string& default_value = "");
    static int GetInt(const std::string& key, int default_value = 0);
    static bool GetBool(const std::string& key, bool default_value = false);
    static void Set(const std::string& key, const std::string& value);
};

} // namespace manimart::util
