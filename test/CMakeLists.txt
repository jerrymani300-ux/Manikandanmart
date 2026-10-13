# Tests CMake Configuration for ManiMart

set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/service/AuthServiceTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/service/ProductServiceTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/service/CartServiceTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/service/OrderServiceTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/service/ReviewServiceTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/repository/RepositoryIntegrationTests.cpp
)

# Additional test files if present
file(GLOB_RECURSE EXTRA_TEST_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/integration/*.cpp")
list(APPEND TEST_SOURCES ${EXTRA_TEST_SRCS})
