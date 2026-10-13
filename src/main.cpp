#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <filesystem>

#include "util/Env.hpp"
#include "util/PasswordHasher.hpp"
#include "util/RateLimiter.hpp"
#include "util/QuestionCache.hpp"

#include "repository/DatabaseConnection.hpp"
#include "repository/RepositoryFactory.hpp"

#include "provider/ProviderFactory.hpp"

#include "service/AuthService.hpp"
#include "service/ProductService.hpp"
#include "service/CartService.hpp"
#include "service/OrderService.hpp"
#include "service/ReviewService.hpp"
#include "service/SellerService.hpp"
#include "service/AdminService.hpp"
#include "service/ChatService.hpp"
#include "service/MigrationService.hpp"

#include "controller/AuthController.hpp"
#include "controller/ProductController.hpp"
#include "controller/CartController.hpp"
#include "controller/OrderController.hpp"
#include "controller/ReviewController.hpp"
#include "controller/SellerController.hpp"
#include "controller/AdminController.hpp"
#include "controller/ChatController.hpp"

using namespace manimart;

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // 1. Initialize Structured Logging
    spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [thread %t] %v");
    spdlog::set_level(spdlog::level::info);
    spdlog::info("==================================================");
    spdlog::info("  Starting ManiMart E-Commerce Marketplace Server  ");
    spdlog::info("==================================================");

    // 2. Load Environment Variables (.env)
    util::Env::LoadDotEnv(".env");

    // 3. Initialize Cryptographic Subsystem (libsodium)
    if (!util::PasswordHasher::Initialize()) {
        spdlog::warn("Failed to initialize libsodium crypto provider, fallback active");
    }

    // 4. Initialize Database & Migrations
    std::string db_type = util::Env::Get("DB_TYPE", "sqlite");
    std::string db_path = util::Env::Get("SQLITE_DB_PATH", "manimart.db");
    spdlog::info("Initializing database (type: {}, path: {})...", db_type, db_path);

    repository::DatabaseConnection::GetInstance().Initialize(db_type, db_path);

    // Run Migrations and Seed
    auto mig_repo = repository::RepositoryFactory::CreateMigrationRepository();
    service::MigrationService mig_service(mig_repo);
    mig_service.RunMigrations("db/migrations");

    auto user_repo = repository::RepositoryFactory::CreateUserRepository();
    if (user_repo->CountAll() == 0) {
        spdlog::info("Database is empty, automatically executing db/seed.sql...");
        mig_service.RunSeed("db/seed.sql");
    }

    // 5. Instantiate Repositories via Factory Pattern
    auto product_repo = repository::RepositoryFactory::CreateProductRepository();
    auto cart_repo = repository::RepositoryFactory::CreateCartRepository();
    auto order_repo = repository::RepositoryFactory::CreateOrderRepository();
    auto review_repo = repository::RepositoryFactory::CreateReviewRepository();
    auto wishlist_repo = repository::RepositoryFactory::CreateWishlistRepository();

    // 6. Instantiate Providers via Factory Pattern
    auto chat_provider = provider::ProviderFactory::CreateChatProvider();
    auto payment_strategy = provider::ProviderFactory::CreatePaymentStrategy();

    // 7. Instantiate Utilities
    auto rate_limiter = std::make_shared<util::RateLimiter>(
        util::Env::GetInt("CHAT_RATE_LIMIT_PER_MINUTE", 10),
        std::chrono::seconds(60)
    );
    auto question_cache = std::make_shared<util::QuestionCache>(std::chrono::minutes(30));

    // 8. Instantiate Services with Constructor Dependency Injection
    auto auth_service = std::make_shared<service::AuthService>(user_repo);
    auto product_service = std::make_shared<service::ProductService>(product_repo, user_repo);
    auto cart_service = std::make_shared<service::CartService>(cart_repo, product_repo);
    auto order_service = std::make_shared<service::OrderService>(order_repo, cart_repo, product_repo, user_repo, payment_strategy);
    auto review_service = std::make_shared<service::ReviewService>(review_repo, order_repo, product_repo);
    auto seller_service = std::make_shared<service::SellerService>(product_repo, order_repo);
    auto admin_service = std::make_shared<service::AdminService>(user_repo, product_repo, order_repo);
    auto chat_service = std::make_shared<service::ChatService>(chat_provider, rate_limiter, question_cache);

    // 9. Wire Services into Controllers
    controller::AuthController::SetAuthService(auth_service);
    controller::ProductController::SetProductService(product_service);
    controller::CartController::SetCartService(cart_service);
    controller::OrderController::SetOrderService(order_service);
    controller::ReviewController::SetReviewService(review_service);
    controller::SellerController::SetServices(seller_service, product_service, order_service);
    controller::AdminController::SetAdminService(admin_service);
    controller::ChatController::SetChatService(chat_service);

    // 10. Configure Drogon HTTP Server
    std::string host = util::Env::Get("SERVER_HOST", "0.0.0.0");
    int port = util::Env::GetInt("PORT", util::Env::GetInt("SERVER_PORT", 8080));
    int threads = util::Env::GetInt("SERVER_THREADS", 4);
    int session_timeout = util::Env::GetInt("SESSION_TIMEOUT_SECONDS", 3600);

    // Check if config.json exists
    if (std::filesystem::exists("config/config.json")) {
        try {
            drogon::app().loadConfigFile("config/config.json");
            spdlog::info("Loaded configuration from config/config.json");
        } catch (const std::exception& ex) {
            spdlog::warn("Could not load config/config.json ({}), applying programmatic settings", ex.what());
        }
    }

    drogon::app()
        .addListener(host, port)
        .setThreadNum(threads)
        .enableSession(session_timeout)
        .setDocumentRoot("./frontend")
        .setHomePage("index.html");

    std::string provider_name = chat_provider->GetProviderName();
    spdlog::info("ManiMart server configured on http://{}:{}", host, port);
    spdlog::info("Document root set to: ./frontend");
    spdlog::info("AI Chatbot provider: {}", provider_name);

    // 11. Run Drogon Application Event Loop
    drogon::app().run();

    return 0;
}
