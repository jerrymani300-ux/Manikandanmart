#pragma once

#include <string>
#include <unordered_map>

namespace Json {

class Value {
public:
    Value() = default;

    std::string asString() const { return str_val_; }
    int asInt() const { return int_val_; }

    Value get(const std::string& key, const Value& defaultValue) const {
        auto it = map_.find(key);
        return it != map_.end() ? it->second : defaultValue;
    }

    Value get(const std::string& key, const std::string& defaultValue) const {
        auto it = map_.find(key);
        if (it != map_.end()) return it->second;
        Value v;
        v.str_val_ = defaultValue;
        return v;
    }

private:
    std::string str_val_;
    int int_val_{0};
    std::unordered_map<std::string, Value> map_;
};

} // namespace Json
