#pragma once

#include <string>

namespace manimart::util {

class PasswordHasher {
public:
    static bool Initialize();

    // Hashes password using Argon2id (libsodium crypto_pwhash)
    static std::string HashPassword(const std::string& password);

    // Verifies password against hashed string in constant time
    static bool VerifyPassword(const std::string& password, const std::string& hash);

    // Validates password complexity: >= 8 chars, 1 uppercase, 1 lowercase, 1 number, 1 special char
    static bool ValidatePasswordStrength(const std::string& password, std::string& out_reason);
};

} // namespace manimart::util
