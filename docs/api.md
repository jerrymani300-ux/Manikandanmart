# ManiMart REST API Specification (`/api/v1/`)

All API responses follow the standard envelope format:

### Standard Success Response
```json
{
  "success": true,
  "data": { ... },
  "error": null
}
```

### Standard Error Response
```json
{
  "success": false,
  "data": null,
  "error": {
    "code": "VALIDATION_ERROR",
    "message": "Descriptive error message",
    "field": "email"
  }
}
```

---

## Endpoint Reference

### 1. Health Probe
- `GET /api/v1/health` -> System health status and DB connectivity check.

### 2. Authentication
- `POST /api/v1/auth/register` -> Register a new Buyer or Seller.
- `POST /api/v1/auth/login` -> Authenticate and initialize server session.
- `POST /api/v1/auth/logout` -> Terminate user session.
- `GET /api/v1/auth/me` -> Get current authenticated user profile.

### 3. Products
- `GET /api/v1/products` -> List products (supports `?search=` and `?category=`).
- `GET /api/v1/products/{id}` -> Product details by ID.
- `POST /api/v1/products` -> Create product listing (Seller only).
- `PUT /api/v1/products/{id}` -> Update product listing (Seller only, ownership verified).
- `DELETE /api/v1/products/{id}` -> Delete product (Seller only, ownership verified).

### 4. Cart
- `GET /api/v1/cart` -> Get user's cart items, subtotal, and total amount.
- `POST /api/v1/cart/items` -> Add item to cart.
- `PUT /api/v1/cart/items/{productId}` -> Update item quantity.
- `DELETE /api/v1/cart/items/{productId}` -> Remove item from cart.

### 5. Orders & Checkout
- `POST /api/v1/orders` -> Atomic checkout from user's active cart.
- `GET /api/v1/orders` -> List user's order history.
- `GET /api/v1/orders/{id}` -> View order details.
- `POST /api/v1/orders/{id}/cancel` -> Cancel pending/confirmed order.

### 6. Reviews & Ratings
- `GET /api/v1/products/{id}/reviews` -> Get verified customer reviews and average rating.
- `POST /api/v1/products/{id}/reviews` -> Submit star rating and comment (verified delivered buyers only).

### 7. Seller Portal
- `GET /api/v1/seller/products` -> View all listings owned by current seller.
- `GET /api/v1/seller/orders` -> Incoming orders containing seller's items.
- `GET /api/v1/seller/dashboard` -> Sales metrics, active listings count, revenue.

### 8. Admin Portal
- `GET /api/v1/admin/dashboard` -> System-wide statistics and metrics.
- `GET /api/v1/admin/users` -> View all registered accounts and roles.
- `GET /api/v1/admin/products` -> View all listings.
- `DELETE /api/v1/admin/products/{id}` -> Moderate/delete inappropriate listing.
- `GET /api/v1/admin/orders` -> View all marketplace orders.

### 9. AI Assistant
- `POST /api/v1/chat` -> AI Chatbot query with domain restriction, rate limiting, and cache.
