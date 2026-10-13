# ManiMart Architecture Specification

## 1. High-Level Architectural Overview

ManiMart follows a strict, decoupled layered architecture:

```
┌─────────────────────────────────────────────────────────────┐
│              Browser Client (HTML5 / CSS3 / Vanilla JS)     │
└──────────────────────────────┬──────────────────────────────┘
                               │ HTTP / JSON
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                       Drogon Filters                        │
│   (LoggingFilter, CorsFilter, AuthFilter, RoleFilter)       │
└──────────────────────────────┬──────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                     Drogon Controllers                      │
│   (Auth, Product, Cart, Order, Review, Seller, Admin, Chat) │
└──────────────────────────────┬──────────────────────────────┘
                               │ Constructor DI
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                       Service Layer                         │
│   (AuthService, ProductService, CartService, OrderService,   │
│    ReviewService, SellerService, AdminService, ChatService) │
└──────────────────────────────┬──────────────────────────────┘
                               │ Constructor DI (Interfaces)
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                      Repository Layer                       │
│  (IUserRepository, IProductRepository, ICartRepository, etc)│
└──────────────────────────────┬──────────────────────────────┘
                               │ RAII Parameterized SQL
                               ▼
┌─────────────────────────────────────────────────────────────┐
│             Database (PostgreSQL / SQLite Connection)       │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. Design Patterns Implemented

1. **Repository Pattern**:
   - `IUserRepository`, `IProductRepository`, `ICartRepository`, `IOrderRepository`, `IReviewRepository`, `IWishlistRepository`, `IMigrationRepository`.
   - Business services interact exclusively with pure virtual interfaces, isolating data persistence details and enabling mock-based unit testing.

2. **Front Controller & Thin Controller Pattern**:
   - Drogon HTTP controllers handle routing, basic JSON deserialization, and HTTP response envelope translation.
   - Controllers never execute SQL or enforce domain logic.

3. **Singleton Pattern**:
   - `DatabaseConnection::GetInstance()` provides centralized, thread-safe connection pooling, transaction orchestration, and SQLite/PostgreSQL statement execution.

4. **Factory Pattern**:
   - `RepositoryFactory`: Centralized creation of concrete repository instances.
   - `ProviderFactory`: Dynamically instantiates `ChatProvider` (Gemini or Mock) and `IPaymentStrategy`.

5. **Strategy Pattern**:
   - `IPaymentStrategy` with `MockPaymentStrategy`: Allows plugging in real card gateways (e.g. Stripe, Razorpay) without altering `OrderService`.

6. **Builder Pattern**:
   - `UserDtoBuilder`, `ProductDtoBuilder`, `OrderDtoBuilder`: Provides fluent, validated construction of response DTOs and filters out sensitive fields like `password_hash`.

7. **Dependency Injection**:
   - All services receive repository interfaces and provider dependencies via constructor injection.

---

## 3. Financial Calculations & Value Objects

Currency in ManiMart is strictly managed through the `manimart::util::Money` value object encapsulating minor units (`int64_t cents`). Floating point data types are prohibited across all financial arithmetic to avoid rounding inaccuracies.
