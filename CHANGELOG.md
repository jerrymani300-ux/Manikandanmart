# Changelog

All notable changes to the ManiMart project are documented in this file.

## [1.0.0] - 2026-09-22
### Added
- **Core Architecture**: Layered architecture featuring Drogon Filters, Controllers, Service layer, and Repository layer with Constructor Dependency Injection.
- **Security**: Libsodium Argon2id password hashing, server-side session management with login regeneration, parameterized SQL queries, and role-based authorization filters.
- **Database & Migrations**: Automated migration runner with V1 to V5 versioned schema changes and realistic seed data.
- **Financial Safety**: Strict `Money` value type encapsulating minor units (`cents`).
- **E-Commerce Flows**: Complete Buyer browsing, search, category filter, cart operations, atomic multi-item checkout, mock payment gateway, order lifecycle, and verified buyer reviews.
- **Seller Portal**: Product catalog CRUD, stock adjustments, and sales metrics dashboard.
- **Admin Center**: Platform-wide metrics, user role auditing, and listing moderation.
- **AI Chatbot**: Multi-provider assistant (Google Gemini API & offline Mock provider) with in-memory sliding-window rate limiting, duplicate question caching, and server-side secret management.
- **Frontend**: Full-featured responsive UI built with modern HTML5, CSS3, and Vanilla JavaScript.
- **Automated Testing & CI**: GoogleTest and GoogleMock unit test suite, repository integration tests, sanitizer build profiles, and GitHub Actions CI workflow.
