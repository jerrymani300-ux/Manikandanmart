# Project Retrospective — ManiMart E-Commerce Marketplace

## What Went Well
- **Decoupled Architecture**: Strict separation of concerns between Drogon HTTP controllers, domain services, repository interfaces, and data connections made unit testing effortless.
- **Financial Precision**: Using the custom `Money` value object (`int64_t cents`) completely eliminated floating-point precision hazards during checkout calculations.
- **Robust Security Posture**: Parameterized SQL statements, Argon2id hashing, server-side sessions, and strict buyer/seller/admin filters ensure comprehensive defense-in-depth.
- **Compulsory AI Chatbot Integration**: Dual-provider strategy with Gemini HTTP streaming and offline mock fallback guarantees 100% testability and reliability even without active API keys.

## Lessons Learned & Future Improvements
- Supporting both SQLite and PostgreSQL allows ultra-fast in-memory unit testing alongside production-grade concurrency.
- Future versions could implement WebSockets for live order status notifications and real-time merchant inventory updates.
