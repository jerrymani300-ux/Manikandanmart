# ManiMart — Modern Multi-Seller E-Commerce Marketplace

[![ManiMart CI](https://github.com/manimart/manimart/actions/workflows/ci.yml/badge.svg)](https://github.com/manimart/manimart/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![Drogon](https://img.shields.io/badge/Framework-Drogon-red.svg)](https://drogon.org)

**ManiMart** is a production-style, full-stack multi-seller e-commerce marketplace web platform engineered using modern **C++20**, the high-performance **Drogon Web Framework**, **PostgreSQL** (with SQLite local/testing support), **libsodium Argon2id** password hashing, and a modern responsive **HTML5/CSS3/Vanilla JS** user interface with an integrated **AI Customer Support Chatbot**.

---

## Key Features

### 🛍️ Buyer Experience
- **Catalog Browsing & Search**: Search by keywords and filter by product categories (Electronics, Fashion, Books, Home, Fitness, Accessories).
- **Product Details & Ratings**: View high-resolution imagery, seller info, real-time inventory status, and verified customer star ratings.
- **Cart Management**: Add products, adjust quantities with live stock checks, remove items, and view server-verified subtotals.
- **Atomic Checkout**: Transaction-safe multi-item order placement, simulated payment verification (Card, UPI, COD), and automatic stock decrement.
- **Order Tracking & Cancellations**: Track orders across `PENDING`, `CONFIRMED`, `SHIPPED`, and `DELIVERED` statuses.
- **Verified Buyer Reviews**: Only buyers who completed and received a delivered purchase can leave star ratings and reviews.

### 🏪 Seller Portal
- **Seller Registration & Authentication**: Distinct merchant account lifecycle.
- **Storefront Inventory CRUD**: Create, view, edit, and delete products with strict ownership checks.
- **Merchant Analytics**: Real-time counters for active listings, total orders, gross revenue, and pending shipments.

### 🛡️ Administrator Center
- **System Moderation**: Remove inappropriate or fraudulent listings.
- **User Auditing**: Monitor registered buyer and seller accounts.
- **Platform Overview**: Global order stream, gross sales volume, and transaction metrics.

### 🤖 AI Assistant Chatbot
- **ManiMart Domain Support**: Answers marketplace, product, checkout, and seller inquiries.
- **Provider Abstraction**: Google Gemini API integration with automatic fallback to offline mock responses.
- **Rate Limiting & Caching**: Mutex-protected sliding window rate limiter (10 req/min) and session question cache.
- **Secure Secrets**: API keys loaded strictly server-side from environment variables.

---

## Architecture & Design Patterns

```
Browser (HTML5/CSS3/JS) -> Drogon Filters -> Drogon Controllers -> Service Layer -> Repository Layer -> Database
```

- **Repository Pattern**: Abstract data interfaces (`IUserRepository`, `IProductRepository`, etc.) isolating database queries.
- **Front Controller & Thin Controller Pattern**: Drogon controllers only deserialize requests and format standard response envelopes.
- **Factory Pattern**: `RepositoryFactory` and `ProviderFactory` instantiate repositories and AI/payment providers.
- **Strategy Pattern**: `IPaymentStrategy` enables interchangeable payment gateways.
- **Builder Pattern**: DTO builders (`ProductDtoBuilder`, `OrderDtoBuilder`, etc.) construct validated output objects.
- **Dependency Injection**: Constructor injection across all services.
- **Money Value Type**: Minor units (`int64_t cents`) preventing float rounding errors.

---

## Demo Credentials (Seeded Database)

| Role | Email | Password |
| :--- | :--- | :--- |
| **Admin** | `admin@manimart.com` | `AdminPass@123` |
| **Seller 1** | `seller1@techhub.com` | `SellerPass@123` |
| **Seller 2** | `seller2@styleco.com` | `SellerPass@123` |
| **Buyer 1** | `buyer1@gmail.com` | `BuyerPass@123` |
| **Buyer 2** | `buyer2@gmail.com` | `BuyerPass@123` |

---

## Directory Structure

```
ManiMart/
├── CMakeLists.txt
├── CMakePresets.json
├── vcpkg.json
├── Dockerfile
├── docker-compose.yml
├── .env.example
│
├── src/
│   ├── main.cpp
│   ├── controller/      (Auth, Product, Cart, Order, Review, Seller, Admin, Chat, Health)
│   ├── service/         (Auth, Product, Cart, Order, Review, Seller, Admin, Chat, Migration)
│   ├── repository/      (Interfaces, DatabaseConnection, SQLite/Postgres SQL implementations)
│   ├── model/           (User, Product, Order, OrderItem, CartItem, Review, WishlistItem)
│   ├── dto/             (Request/Response DTOs, Builders, EnvelopeDto)
│   ├── filter/          (LoggingFilter, AuthFilter, RoleFilter, CorsFilter)
│   ├── plugin/          (DatabasePlugin)
│   ├── util/            (Money, PasswordHasher, ValidationUtil, RateLimiter, QuestionCache, Env)
│   ├── exception/       (AppException, ValidationException, AuthenticationException, etc.)
│   └── provider/        (ChatProvider, GeminiChatProvider, MockChatProvider, PaymentStrategy)
│
├── config/
│   ├── config.json
│   └── config.example.json
│
├── db/
│   ├── schema.sql
│   ├── seed.sql
│   └── migrations/      (V1 to V5 versioned SQL migrations)
│
├── frontend/
│   ├── index.html, products.html, product.html, login.html, register.html
│   ├── cart.html, checkout.html, orders.html, seller.html, admin.html, profile.html
│   ├── css/style.css
│   └── js/ (api.js, auth.js, products.js, cart.js, checkout.js, orders.js, seller.js, admin.js, chatbot.js)
│
├── test/
│   ├── service/ (Auth, Product, Cart, Order, Review unit tests)
│   ├── repository/ (Integration tests against SQLite schema)
│   └── test_helpers/ (GoogleMock repositories)
│
└── docs/
    ├── architecture.md, api.md, database.md, security.md, testing.md, deployment.md
```

---

## Running ManiMart

### 🚀 One-Click Run (Windows)
From the project folder, simply **double-click** `run.bat` or execute in terminal:
```cmd
run.bat
```
This automatically:
1. Configures required toolchain and runtime library paths.
2. Builds the project binaries if not already built.
3. Automatically applies database migrations and seeds demo accounts.
4. Starts the backend HTTP server on `http://127.0.0.1:8080`.
5. Automatically opens `http://127.0.0.1:8080/` in your default browser.

---

### 💻 Manual Build & Run Instructions

#### 1. Build using CMake & Ninja (GCC 16 / MinGW-w64)
```powershell
$env:PATH = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;" + $env:PATH
cmake -B build_test -G "Ninja" -DCMAKE_CXX_COMPILER="C:/msys64/ucrt64/bin/g++.exe" -DCMAKE_C_COMPILER="C:/msys64/ucrt64/bin/gcc.exe"
cmake --build build_test
```

#### 2. Run the Test Suite (18/18 GoogleTest Unit & Integration Tests)
```powershell
$env:PATH = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;" + $env:PATH
.\build_test\manimart_tests.exe
```

#### 3. Start the Backend Server
```powershell
$env:PATH = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;" + $env:PATH
.\build_test\manimart.exe
```

---

## 🌐 Endpoints & URLs

- **Website / Storefront**: [http://127.0.0.1:8080/](http://127.0.0.1:8080/)
- **Health Check**: [http://127.0.0.1:8080/api/v1/health](http://127.0.0.1:8080/api/v1/health)
- **Product Catalog API**: [http://127.0.0.1:8080/api/v1/products](http://127.0.0.1:8080/api/v1/products)
- **AI Domain Assistant**: [http://127.0.0.1:8080/api/v1/chat](http://127.0.0.1:8080/api/v1/chat)

---

## 🔑 Demo Accounts

| Role | Email | Password | What You Can Test |
| :--- | :--- | :--- | :--- |
| **Buyer** | `buyer1@gmail.com` | `BuyerPass@123` | Browse catalog, search, add to cart, atomic checkout, order tracking, verified reviews. |
| **Seller** | `seller1@techhub.com` | `SellerPass@123` | Seller portal, product inventory CRUD, revenue analytics, order fulfillment. |
| **Admin** | `admin@manimart.com` | `AdminPass@123` | Admin center, system-wide metrics, user auditing, product listing moderation. |

---

## 🐳 Docker Quickstart

```bash
docker-compose up -d --build
```
Open **http://localhost:8080**.

---

## License
MIT License. See [LICENSE](LICENSE) for details.
