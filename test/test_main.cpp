#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include "util/PasswordHasher.hpp"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    spdlog::set_level(spdlog::level::warn);
    manimart::util::PasswordHasher::Initialize();
    return RUN_ALL_TESTS();
}
