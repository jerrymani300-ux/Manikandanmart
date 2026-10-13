# ManiMart Security Architecture

## 1. Authentication & Password Security
- **Argon2id Hashing**: Password hashing uses libsodium's `crypto_pwhash` (Argon2id) with unique random salts and constant-time password verification.
- **Zero Hash Leakage**: Password hashes are never returned across API DTOs or written to application logs.
- **Session Management**: Drogon server-side sessions with pre-auth session invalidation upon login and configurable idle timeouts.

## 2. SQL Injection Prevention
- All SQL operations in the repository layer utilize strictly parameterized queries and prepared statements.
- Direct string concatenation into SQL queries is forbidden.

## 3. Authorization & Access Control
- Role-based authorization enforced via `AuthFilter`, `SellerFilter`, and `AdminFilter`.
- Product mutation requires strict ownership verification (sellers can only edit/delete their own listings).
- Order inspection restricted to the purchasing buyer or system administrators.
- Reviews restricted to verified buyers who purchased the product through a completed, delivered order.

## 4. Input Validation & XSS Defense
- Strict input validation in `ValidationUtil` rejecting malformed inputs prior to repository invocation.
- HTML sanitization and escaping for user-supplied strings.

## 5. Secret Management & AI Security
- API keys (e.g. `GEMINI_API_KEY`) loaded exclusively from server environment variables and never exposed to the frontend.
- Rate limiting (10 requests/min per session) and prompt templating to prevent prompt injection and resource abuse.
