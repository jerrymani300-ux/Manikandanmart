# ManiMart Database Specification

## 1. Schema Design

The database schema enforces relational integrity across all tables using foreign keys, check constraints, unique constraints, and indexes.

### Core Tables:
1. `schema_migrations`: Tracks applied migration versions (`version`, `description`, `applied_at`).
2. `users`: Stores user accounts (`id`, `name`, `email UNIQUE`, `password_hash`, `role CHECK ('BUYER', 'SELLER', 'ADMIN')`, `created_at`).
3. `products`: Stores seller inventory (`id`, `seller_id FK`, `name`, `description`, `price_cents BIGINT`, `stock_qty INT`, `category`, `image_url`, `created_at`).
4. `orders`: Stores orders (`id`, `buyer_id FK`, `status CHECK ('PENDING', 'CONFIRMED', 'SHIPPED', 'DELIVERED', 'CANCELLED')`, `total_amount_cents BIGINT`, `created_at`).
5. `order_items`: Line items (`id`, `order_id FK`, `product_id FK`, `quantity INT`, `unit_price_cents BIGINT`, `created_at`).
6. `cart_items`: Shopping carts (`id`, `user_id FK`, `product_id FK`, `quantity INT`, `created_at`, `UNIQUE(user_id, product_id)`).
7. `reviews`: Customer reviews (`id`, `product_id FK`, `user_id FK`, `rating CHECK (1..5)`, `comment`, `created_at`, `UNIQUE(product_id, user_id)`).
8. `wishlist`: User wishlists (`id`, `user_id FK`, `product_id FK`, `created_at`, `UNIQUE(user_id, product_id)`).
9. `chat_logs`: Audit log for AI assistant queries (`id`, `session_id`, `user_message`, `bot_response`, `provider`, `created_at`).

---

## 2. Versioned Migrations

- `V1__init_schema.sql`: Core schema (users, products, orders, order_items, cart_items).
- `V2__add_reviews_table.sql`: Reviews and ratings table.
- `V3__add_order_indexes.sql`: Performance indexes on all Foreign Keys.
- `V4__add_wishlist_table.sql`: Wishlist table.
- `V5__add_chatbot_support.sql`: Chat audit logs and indexing.

---

## 3. Atomic Transactions

Checkout operations run in a single atomic database transaction:
1. Stock check
2. Order creation
3. Order items insertion
4. Inventory stock decrement
5. Cart clearance
