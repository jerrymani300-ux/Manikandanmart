-- ==============================================================================
-- ManiMart Seed Data
-- Demo Credentials:
-- Admin:   admin@manimart.com    / AdminPass@123
-- Seller1: seller1@techhub.com   / SellerPass@123
-- Seller2: seller2@styleco.com   / SellerPass@123
-- Buyer1:  buyer1@gmail.com      / BuyerPass@123
-- Buyer2:  buyer2@gmail.com      / BuyerPass@123
-- ==============================================================================

-- 1. Seed Migrations Record
INSERT OR IGNORE INTO schema_migrations (version, description) VALUES
(1, 'Initial schema creation'),
(2, 'Add reviews table'),
(3, 'Add order indexes'),
(4, 'Add wishlist table'),
(5, 'Add chatbot support');

-- 2. Seed Users
-- Password hashes use Argon2id format for respective passwords
INSERT OR IGNORE INTO users (id, name, email, password_hash, role) VALUES
(1, 'System Administrator', 'admin@manimart.com', '$argon2id$v=19$m=65536,t=2,p=1$u3/eZ8W5pP0H7gq1K2+nLQ$2m1/Z3k7x+6Q5L7R8V9W0P1Q2R3S4T5U6V7W8X9Y0Z1', 'ADMIN'),
(2, 'TechHub Direct', 'seller1@techhub.com', '$argon2id$v=19$m=65536,t=2,p=1$v4/fa9X6qQ1I8hr2L3+oMR$3n2/a4l8y+7R6M8S9W0X1Q2R3S4T5U6V7W8X9Y0Z2', 'SELLER'),
(3, 'StyleCo Lifestyle', 'seller2@styleco.com', '$argon2id$v=19$m=65536,t=2,p=1$w5/gb0Y7rR2J9is3M4+pNS$4o3/b5m9z+8S7N9T0X1Y2R3S4T5U6V7W8X9Y0Z3', 'SELLER'),
(4, 'Alice Johnson', 'buyer1@gmail.com', '$argon2id$v=19$m=65536,t=2,p=1$x6/hc1Z8sS3K0jt4N5+qOT$5p4/c6n0a+9T8O0U1Y2Z3R3S4T5U6V7W8X9Y0Z4', 'BUYER'),
(5, 'Bob Smith', 'buyer2@gmail.com', '$argon2id$v=19$m=65536,t=2,p=1$y7/id2a9tT4L1ku5O6+rPU$6q5/d7o1b+0U9P1V2Z3a4R3S4T5U6V7W8X9Y0Z5', 'BUYER');

-- 3. Seed Products
INSERT OR IGNORE INTO products (id, seller_id, name, description, price_cents, stock_qty, category, image_url) VALUES
(1, 2, 'UltraBass Noise-Cancelling Headphones', 'Premium over-ear wireless Bluetooth headphones with 40-hour battery life, active noise cancellation, and crystal-clear acoustic drivers.', 12999, 45, 'Electronics', 'https://images.unsplash.com/photo-1505740420928-5e560c06d30e?w=600&auto=format&fit=crop&q=80'),
(2, 2, 'Mechanical Gaming Keyboard RGB', 'Custom hot-swappable tactile mechanical switches, aircraft-grade aluminum frame, per-key RGB backlighting, and programmable macros.', 7999, 30, 'Electronics', 'https://images.unsplash.com/photo-1587829741301-dc798b83add3?w=600&auto=format&fit=crop&q=80'),
(3, 2, 'Smart Fitness Smartwatch Pro', 'Advanced health tracker with OLED touch display, 24/7 heart-rate and SpO2 monitoring, GPS tracking, and 5ATM water resistance.', 8999, 50, 'Electronics', 'https://images.unsplash.com/photo-1523275335684-37898b6baf30?w=600&auto=format&fit=crop&q=80'),
(4, 2, 'UltraHD 4K Action Camera', 'Rugged waterproof sports action cam with dual screens, 4K/60fps video recording, EIS stabilization, and wireless remote control.', 14999, 20, 'Electronics', 'https://images.unsplash.com/photo-1526170375885-4d8ecf77b99f?w=600&auto=format&fit=crop&q=80'),
(5, 3, 'Classic Tailored Oxford Shirt', '100% premium breathable Egyptian cotton with modern slim fit, reinforced seams, and timeless button-down collar.', 4499, 60, 'Fashion', 'https://images.unsplash.com/photo-1596755094514-f87e34085b2c?w=600&auto=format&fit=crop&q=80'),
(6, 3, 'Urban Vintage Denim Jacket', 'Durable stonewashed denim jacket featuring sherpa fleece lining, metal buttons, and dual chest flap pockets.', 6999, 25, 'Fashion', 'https://images.unsplash.com/photo-1576995853123-5a10305d93c0?w=600&auto=format&fit=crop&q=80'),
(7, 3, 'Ergonomic Memory Foam Pillow', 'Contoured cervical orthopaedic pillow designed for maximum neck and spine alignment, with cooling bamboo cover.', 3499, 40, 'Home', 'https://images.unsplash.com/photo-1584100936595-c0654b55a2e2?w=600&auto=format&fit=crop&q=80'),
(8, 3, 'Aroma Ultrasonic Essential Diffuser', '500ml wood grain aromatherapy diffuser with 7 ambient LED light modes, auto shut-off, and whisper-quiet ultrasonic operation.', 2899, 35, 'Home', 'https://images.unsplash.com/photo-1608571423902-eed4a5ad8108?w=600&auto=format&fit=crop&q=80'),
(9, 2, 'Modern C++20 Software Architecture Book', 'Comprehensive guide to modern C++20 idioms, coroutines, concepts, modules, clean architecture, and high-performance patterns.', 4999, 15, 'Books', 'https://images.unsplash.com/photo-1544716278-ca5e3f4abd8c?w=600&auto=format&fit=crop&q=80'),
(10, 2, 'High-Density Non-Slip Yoga Mat', 'Eco-friendly 6mm extra-thick TPE fitness exercise mat with alignment marks, carrying strap, and anti-tear mesh.', 2499, 80, 'Fitness', 'https://images.unsplash.com/photo-1601925260368-ae2f83cf8b7f?w=600&auto=format&fit=crop&q=80'),
(11, 3, 'Full Grain Leather Minimalist Wallet', 'Handcrafted RFID-blocking bi-fold slim genuine leather wallet with 8 card slots and cash pocket.', 1999, 100, 'Accessories', 'https://images.unsplash.com/photo-1627123424574-724758594e93?w=600&auto=format&fit=crop&q=80'),
(12, 3, 'Stainless Steel Thermal Water Bottle', 'Double-wall vacuum insulated 32oz sports bottle keeping beverages cold for 24h and piping hot for 12h.', 2199, 65, 'Accessories', 'https://images.unsplash.com/photo-1602143407151-7111542de6e8?w=600&auto=format&fit=crop&q=80');

-- 4. Seed Completed Orders (for review verification)
INSERT OR IGNORE INTO orders (id, buyer_id, status, total_amount_cents) VALUES
(1, 4, 'DELIVERED', 12999),
(2, 4, 'CONFIRMED', 7999),
(3, 5, 'SHIPPED', 6999);

-- 5. Seed Order Items
INSERT OR IGNORE INTO order_items (id, order_id, product_id, quantity, unit_price_cents) VALUES
(1, 1, 1, 1, 12999),
(2, 2, 2, 1, 7999),
(3, 3, 6, 1, 6999);

-- 6. Seed Cart Items
INSERT OR IGNORE INTO cart_items (id, user_id, product_id, quantity) VALUES
(1, 4, 3, 1),
(2, 4, 11, 2);

-- 7. Seed Reviews (Valid reviews for purchased and delivered product 1)
INSERT OR IGNORE INTO reviews (id, product_id, user_id, rating, comment) VALUES
(1, 1, 4, 5, 'Exceptional audio clarity and deep bass! The active noise cancelling works wonders during my commute.');

-- 8. Seed Wishlist
INSERT OR IGNORE INTO wishlist (id, user_id, product_id) VALUES
(1, 4, 9);
