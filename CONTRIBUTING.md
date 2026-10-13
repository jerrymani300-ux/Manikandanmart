# Contributing to ManiMart

Thank you for your interest in contributing to ManiMart!

## Guidelines
1. **Modern C++20**: Use modern C++20 features (concepts, ranges, designated initializers, structured bindings).
2. **Architecture**: Adhere strictly to the layered architecture (Filters -> Controllers -> Services -> Repositories).
3. **Database & Money**: Always use parameterized queries; all financial figures must use the `Money` value type minor units (`int64_t cents`).
4. **Testing**: Write unit tests with GoogleTest/GoogleMock for every new service or feature.
5. **Code Style**: Follow `clang-format` conventions.
