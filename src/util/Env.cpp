#include "util/Env.hpp"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

namespace manimart::util {

void Env::LoadDotEnv(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Strip carriage returns and leading spaces
        auto start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos || line[start] == '#') {
            continue;
        }

        size_t eq_pos = line.find('=', start);
        if (eq_pos == std::string::npos) {
            continue;
        }

        std::string key = line.substr(start, eq_pos - start);
        std::string val = line.substr(eq_pos + 1);

        // Trim key and value
        auto key_end = key.find_last_not_of(" \t\r\n");
        if (key_end != std::string::npos) key = key.substr(0, key_end + 1);

        auto val_start = val.find_first_not_of(" \t\r\n");
        if (val_start != std::string::npos) {
            auto val_end = val.find_last_not_of(" \t\r\n");
            val = val.substr(val_start, val_end - val_start + 1);
            // Strip surrounding quotes
            if (val.length() >= 2 && ((val.front() == '"' && val.back() == '"') || (val.front() == '\'' && val.back() == '\''))) {
                val = val.substr(1, val.length() - 2);
            }
        } else {
            val = "";
        }

        Set(key, val);
    }
}

std::string Env::Get(const std::string& key, const std::string& default_value) {
    const char* val = std::getenv(key.c_str());
    if (val != nullptr && std::string(val).length() > 0) {
        return std::string(val);
    }
    return default_value;
}

int Env::GetInt(const std::string& key, int default_value) {
    std::string s = Get(key, "");
    if (s.empty()) return default_value;
    try {
        return std::stoi(s);
    } catch (...) {
        return default_value;
    }
}

bool Env::GetBool(const std::string& key, bool default_value) {
    std::string s = Get(key, "");
    if (s.empty()) return default_value;
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return (lower == "1" || lower == "true" || lower == "yes" || lower == "on");
}

void Env::Set(const std::string& key, const std::string& value) {
#if defined(_WIN32)
    _putenv_s(key.c_str(), value.c_str());
#else
    setenv(key.c_str(), value.c_str(), 1);
#endif
}

} // namespace manimart::util
