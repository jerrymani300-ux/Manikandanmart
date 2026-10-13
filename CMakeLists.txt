cmake_minimum_required(VERSION 3.25)
project(manimart VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# Options
option(BUILD_TESTING "Build the testing tree." ON)
option(ENABLE_ASAN "Enable AddressSanitizer." OFF)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer." OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer." OFF)

# Compiler warnings and options
if(MSVC)
    add_compile_options(/W4 /permissive- /Zc:__cplusplus /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wnon-virtual-dtor)
    if(ENABLE_ASAN)
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    endif()
    if(ENABLE_UBSAN)
        add_compile_options(-fsanitize=undefined)
        add_link_options(-fsanitize=undefined)
    endif()
    if(ENABLE_TSAN)
        add_compile_options(-fsanitize=thread)
        add_link_options(-fsanitize=thread)
    endif()
endif()

# Find Threads
find_package(Threads REQUIRED)

# 1. spdlog
find_package(spdlog CONFIG QUIET)
if(NOT spdlog_FOUND)
    message(STATUS "spdlog not found locally, fetching via FetchContent...")
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.12.0
    )
    FetchContent_MakeAvailable(spdlog)
endif()

# 2. nlohmann_json
find_package(nlohmann_json CONFIG QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found locally, fetching via FetchContent...")
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# 3. SQLite3
find_package(SQLite3 QUIET)
if(NOT SQLite3_FOUND)
    message(STATUS "SQLite3 not found locally, fetching sqlite amalgamation...")
    FetchContent_Declare(
        sqlite3_amalgamation
        URL https://www.sqlite.org/2024/sqlite-amalgamation-3450200.zip
    )
    FetchContent_MakeAvailable(sqlite3_amalgamation)
    if(EXISTS "${sqlite3_amalgamation_SOURCE_DIR}/sqlite3.c")
        add_library(sqlite3_lib STATIC "${sqlite3_amalgamation_SOURCE_DIR}/sqlite3.c")
        target_include_directories(sqlite3_lib PUBLIC "${sqlite3_amalgamation_SOURCE_DIR}")
        include_directories("${sqlite3_amalgamation_SOURCE_DIR}")
    endif()
endif()

# 4. Drogon
find_package(Drogon CONFIG QUIET)
if(NOT Drogon_FOUND)
    message(STATUS "Using Drogon compatibility headers from src/compat...")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/compat)
endif()

# 5. libsodium
find_package(sodium CONFIG QUIET)
if(NOT sodium_FOUND)
    find_package(Sodium QUIET)
endif()

# 6. PostgreSQL
find_package(PostgreSQL QUIET)

# 7. cURL
find_package(CURL QUIET)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Core Library Sources
file(GLOB_RECURSE SRC_CORE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/model/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dto/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/util/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/exception/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/repository/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/provider/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/service/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/filter/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/controller/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/plugin/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/compat/drogon/*.cpp"
)

add_library(manimart_core STATIC ${SRC_CORE})
target_include_directories(manimart_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(manimart_core PUBLIC
    Threads::Threads
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

if(TARGET CURL::libcurl)
    target_link_libraries(manimart_core PUBLIC CURL::libcurl)
elseif(CURL_FOUND)
    target_include_directories(manimart_core PUBLIC ${CURL_INCLUDE_DIRS})
    target_link_libraries(manimart_core PUBLIC ${CURL_LIBRARIES})
else()
    target_link_libraries(manimart_core PUBLIC -lcurl)
endif()

if(TARGET Drogon::Drogon)
    target_link_libraries(manimart_core PUBLIC Drogon::Drogon)
endif()

if(TARGET sodium)
    target_link_libraries(manimart_core PUBLIC sodium)
elseif(SODIUM_FOUND)
    target_include_directories(manimart_core PUBLIC ${SODIUM_INCLUDE_DIRS})
    target_link_libraries(manimart_core PUBLIC ${SODIUM_LIBRARIES})
endif()

if(TARGET SQLite::SQLite3)
    target_link_libraries(manimart_core PUBLIC SQLite::SQLite3)
elseif(SQLITE3_FOUND)
    target_include_directories(manimart_core PUBLIC ${SQLITE3_INCLUDE_DIRS})
    target_link_libraries(manimart_core PUBLIC ${SQLITE3_LIBRARIES})
elseif(TARGET sqlite3_lib)
    target_link_libraries(manimart_core PUBLIC sqlite3_lib)
endif()

if(PostgreSQL_FOUND)
    target_include_directories(manimart_core PUBLIC ${PostgreSQL_INCLUDE_DIRS})
    target_link_libraries(manimart_core PUBLIC ${PostgreSQL_LIBRARIES})
endif()

if(WIN32)
    target_link_libraries(manimart_core PUBLIC ws2_32 mswsock)
endif()

# Executable
add_executable(manimart src/main.cpp)
target_link_libraries(manimart PRIVATE manimart_core)

# Tests
if(BUILD_TESTING)
    enable_testing()
    find_package(GTest CONFIG QUIET)
    if(NOT GTest_FOUND)
        message(STATUS "GTest not found locally, fetching via FetchContent...")
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG v1.14.0
        )
        FetchContent_MakeAvailable(googletest)
    endif()

    file(GLOB_RECURSE TEST_SRCS
        "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
    )

    add_executable(manimart_tests ${TEST_SRCS})
    target_include_directories(manimart_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
    if(NOT Drogon_FOUND)
        target_include_directories(manimart_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/compat)
    endif()
    target_link_libraries(manimart_tests PRIVATE
        manimart_core
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
    )
    add_test(NAME ManiMartTests COMMAND manimart_tests)
endif()
