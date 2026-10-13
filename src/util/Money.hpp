#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <compare>
#include <nlohmann/json.hpp>

namespace manimart::util {

class Money {
public:
    constexpr Money() noexcept : cents_(0) {}
    constexpr explicit Money(int64_t cents) noexcept : cents_(cents) {}

    [[nodiscard]] constexpr int64_t GetCents() const noexcept { return cents_; }
    [[nodiscard]] constexpr bool IsZero() const noexcept { return cents_ == 0; }
    [[nodiscard]] constexpr bool IsPositive() const noexcept { return cents_ > 0; }
    [[nodiscard]] constexpr bool IsNegative() const noexcept { return cents_ < 0; }

    [[nodiscard]] std::string ToString() const;
    [[nodiscard]] std::string ToFormattedString() const; // e.g. "$12.99"

    static Money FromCents(int64_t cents);
    static Money FromString(const std::string& str); // Parses "$12.99" or "12.99"

    constexpr auto operator<=>(const Money& other) const noexcept = default;
    constexpr bool operator==(const Money& other) const noexcept = default;

    Money operator+(const Money& other) const;
    Money operator-(const Money& other) const;
    Money operator*(int64_t multiplier) const;
    Money operator/(int64_t divisor) const;

    Money& operator+=(const Money& other);
    Money& operator-=(const Money& other);

    friend std::ostream& operator<<(std::ostream& os, const Money& money);

private:
    int64_t cents_{0};
};

void to_json(nlohmann::json& j, const Money& m);
void from_json(const nlohmann::json& j, Money& m);

} // namespace manimart::util
