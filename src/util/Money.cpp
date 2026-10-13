#include "util/Money.hpp"
#include "exception/AppException.hpp"
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>

namespace manimart::util {

std::string Money::ToString() const {
    std::ostringstream oss;
    oss << (cents_ / 100) << '.' << std::setw(2) << std::setfill('0') << std::abs(cents_ % 100);
    return oss.str();
}

std::string Money::ToFormattedString() const {
    if (cents_ < 0) {
        std::ostringstream oss;
        oss << "-$" << (std::abs(cents_) / 100) << '.' << std::setw(2) << std::setfill('0') << (std::abs(cents_) % 100);
        return oss.str();
    }
    return "$" + ToString();
}

Money Money::FromCents(int64_t cents) {
    return Money(cents);
}

Money Money::FromString(const std::string& str) {
    if (str.empty()) {
        return Money(0);
    }
    std::string s = str;
    // Remove '$', whitespace, commas
    s.erase(std::remove_if(s.begin(), s.end(), [](char c) {
        return c == '$' || c == ' ' || c == ',';
    }), s.end());

    if (s.empty()) {
        return Money(0);
    }

    bool negative = false;
    if (s.front() == '-') {
        negative = true;
        s = s.substr(1);
    }

    size_t dot_pos = s.find('.');
    int64_t dollars = 0;
    int64_t cents = 0;

    try {
        if (dot_pos == std::string::npos) {
            dollars = std::stoll(s);
        } else {
            std::string dollar_str = s.substr(0, dot_pos);
            std::string cent_str = s.substr(dot_pos + 1);
            if (!dollar_str.empty()) {
                dollars = std::stoll(dollar_str);
            }
            if (cent_str.length() == 1) {
                cent_str += "0";
            } else if (cent_str.length() > 2) {
                cent_str = cent_str.substr(0, 2);
            }
            if (!cent_str.empty()) {
                cents = std::stoll(cent_str);
            }
        }
    } catch (...) {
        throw exception::ValidationException("Invalid money format: " + str);
    }

    int64_t total_cents = dollars * 100 + cents;
    if (negative) {
        total_cents = -total_cents;
    }
    return Money(total_cents);
}

Money Money::operator+(const Money& other) const {
    return Money(cents_ + other.cents_);
}

Money Money::operator-(const Money& other) const {
    return Money(cents_ - other.cents_);
}

Money Money::operator*(int64_t multiplier) const {
    return Money(cents_ * multiplier);
}

Money Money::operator/(int64_t divisor) const {
    if (divisor == 0) {
        throw exception::ValidationException("Division by zero in Money calculation");
    }
    return Money(cents_ / divisor);
}

Money& Money::operator+=(const Money& other) {
    cents_ += other.cents_;
    return *this;
}

Money& Money::operator-=(const Money& other) {
    cents_ -= other.cents_;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Money& money) {
    os << money.ToFormattedString();
    return os;
}

void to_json(nlohmann::json& j, const Money& m) {
    j = {
        {"cents", m.GetCents()},
        {"formatted", m.ToFormattedString()}
    };
}

void from_json(const nlohmann::json& j, Money& m) {
    if (j.is_number()) {
        m = Money(j.get<int64_t>());
    } else if (j.is_string()) {
        m = Money::FromString(j.get<std::string>());
    } else if (j.is_object() && j.contains("cents")) {
        m = Money(j["cents"].get<int64_t>());
    } else {
        m = Money(0);
    }
}

} // namespace manimart::util
