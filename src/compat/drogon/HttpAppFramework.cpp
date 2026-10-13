#include "drogon/drogon.h"
#include "controller/HealthController.hpp"
#include "controller/AuthController.hpp"
#include "controller/ProductController.hpp"
#include "controller/CartController.hpp"
#include "controller/OrderController.hpp"
#include "controller/ReviewController.hpp"
#include "controller/SellerController.hpp"
#include "controller/AdminController.hpp"
#include "controller/ChatController.hpp"

#ifdef SendMessage
    #undef SendMessage
#endif

namespace drogon {

void HttpAppFramework::RegisterDefaultRoutes() {
    auto& server = HttpServer::GetInstance();

    // 1. Health
    server.RegisterRoute(Get, "/api/v1/health", HttpServer::AUTH_NONE, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::HealthController controller;
        controller.GetHealth(req, std::move(cb));
    });

    // 2. Auth
    server.RegisterRoute(Post, "/api/v1/auth/register", HttpServer::AUTH_NONE, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::AuthController controller;
        controller.Register(req, std::move(cb));
    });
    server.RegisterRoute(Post, "/api/v1/auth/login", HttpServer::AUTH_NONE, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::AuthController controller;
        controller.Login(req, std::move(cb));
    });
    server.RegisterRoute(Post, "/api/v1/auth/logout", HttpServer::AUTH_NONE, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::AuthController controller;
        controller.Logout(req, std::move(cb));
    });
    server.RegisterRoute(Get, "/api/v1/auth/me", HttpServer::AUTH_NONE, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::AuthController controller;
        controller.GetMe(req, std::move(cb));
    });

    // 3. Products
    server.RegisterRoute(Get, "/api/v1/products", HttpServer::AUTH_NONE, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::ProductController controller;
        controller.GetProducts(req, std::move(cb));
    });
    server.RegisterRoute(Get, "/api/v1/products/{id}", HttpServer::AUTH_NONE, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        std::smatch m;
        std::regex re(R"(/api/v1/products/([0-9]+))");
        std::string p = req->getPath();
        if (std::regex_match(p, m, re)) {
            int64_t id = std::stoll(m[1].str());
            manimart::controller::ProductController controller;
            controller.GetProductById(req, std::move(cb), id);
        }
    });
    server.RegisterRoute(Post, "/api/v1/products", HttpServer::AUTH_SELLER_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::ProductController controller;
        controller.CreateProduct(req, std::move(cb));
    });
    server.RegisterRoute(Put, "/api/v1/products/{id}", HttpServer::AUTH_SELLER_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        std::smatch m;
        std::regex re(R"(/api/v1/products/([0-9]+))");
        std::string p = req->getPath();
        if (std::regex_match(p, m, re)) {
            int64_t id = std::stoll(m[1].str());
            manimart::controller::ProductController controller;
            controller.UpdateProduct(req, std::move(cb), id);
        }
    });
    server.RegisterRoute(Delete, "/api/v1/products/{id}", HttpServer::AUTH_SELLER_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        std::smatch m;
        std::regex re(R"(/api/v1/products/([0-9]+))");
        std::string p = req->getPath();
        if (std::regex_match(p, m, re)) {
            int64_t id = std::stoll(m[1].str());
            manimart::controller::ProductController controller;
            controller.DeleteProduct(req, std::move(cb), id);
        }
    });

    // 4. Cart
    server.RegisterRoute(Get, "/api/v1/cart", HttpServer::AUTH_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::CartController controller;
        controller.GetCart(req, std::move(cb));
    });
    server.RegisterRoute(Post, "/api/v1/cart/items", HttpServer::AUTH_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::CartController controller;
        controller.AddToCart(req, std::move(cb));
    });
    server.RegisterRoute(Put, "/api/v1/cart/items/{id}", HttpServer::AUTH_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        std::smatch m;
        std::regex re(R"(/api/v1/cart/items/([0-9]+))");
        std::string p = req->getPath();
        if (std::regex_match(p, m, re)) {
            int64_t id = std::stoll(m[1].str());
            manimart::controller::CartController controller;
            controller.UpdateCartItem(req, std::move(cb), id);
        }
    });
    server.RegisterRoute(Delete, "/api/v1/cart/items/{id}", HttpServer::AUTH_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        std::smatch m;
        std::regex re(R"(/api/v1/cart/items/([0-9]+))");
        std::string p = req->getPath();
        if (std::regex_match(p, m, re)) {
            int64_t id = std::stoll(m[1].str());
            manimart::controller::CartController controller;
            controller.RemoveCartItem(req, std::move(cb), id);
        }
    });

    // 5. Orders
    server.RegisterRoute(Post, "/api/v1/orders", HttpServer::AUTH_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::OrderController controller;
        controller.Checkout(req, std::move(cb));
    });
    server.RegisterRoute(Get, "/api/v1/orders", HttpServer::AUTH_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::OrderController controller;
        controller.GetOrders(req, std::move(cb));
    });
    server.RegisterRoute(Get, "/api/v1/orders/{id}", HttpServer::AUTH_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        std::smatch m;
        std::regex re(R"(/api/v1/orders/([0-9]+))");
        std::string p = req->getPath();
        if (std::regex_match(p, m, re)) {
            int64_t id = std::stoll(m[1].str());
            manimart::controller::OrderController controller;
            controller.GetOrderById(req, std::move(cb), id);
        }
    });
    server.RegisterRoute(Post, "/api/v1/orders/{id}/cancel", HttpServer::AUTH_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        std::smatch m;
        std::regex re(R"(/api/v1/orders/([0-9]+)/cancel)");
        std::string p = req->getPath();
        if (std::regex_match(p, m, re)) {
            int64_t id = std::stoll(m[1].str());
            manimart::controller::OrderController controller;
            controller.CancelOrder(req, std::move(cb), id);
        }
    });

    // 6. Reviews
    server.RegisterRoute(Get, "/api/v1/products/{id}/reviews", HttpServer::AUTH_NONE, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        std::smatch m;
        std::regex re(R"(/api/v1/products/([0-9]+)/reviews)");
        std::string p = req->getPath();
        if (std::regex_match(p, m, re)) {
            int64_t id = std::stoll(m[1].str());
            manimart::controller::ReviewController controller;
            controller.GetReviews(req, std::move(cb), id);
        }
    });
    server.RegisterRoute(Post, "/api/v1/products/{id}/reviews", HttpServer::AUTH_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        std::smatch m;
        std::regex re(R"(/api/v1/products/([0-9]+)/reviews)");
        std::string p = req->getPath();
        if (std::regex_match(p, m, re)) {
            int64_t id = std::stoll(m[1].str());
            manimart::controller::ReviewController controller;
            controller.AddReview(req, std::move(cb), id);
        }
    });

    // 7. Seller
    server.RegisterRoute(Get, "/api/v1/seller/products", HttpServer::AUTH_SELLER_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::SellerController controller;
        controller.GetProducts(req, std::move(cb));
    });
    server.RegisterRoute(Get, "/api/v1/seller/orders", HttpServer::AUTH_SELLER_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::SellerController controller;
        controller.GetOrders(req, std::move(cb));
    });
    server.RegisterRoute(Get, "/api/v1/seller/dashboard", HttpServer::AUTH_SELLER_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::SellerController controller;
        controller.GetDashboard(req, std::move(cb));
    });

    // 8. Admin
    server.RegisterRoute(Get, "/api/v1/admin/dashboard", HttpServer::AUTH_ADMIN_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::AdminController controller;
        controller.GetDashboard(req, std::move(cb));
    });
    server.RegisterRoute(Get, "/api/v1/admin/users", HttpServer::AUTH_ADMIN_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::AdminController controller;
        controller.GetUsers(req, std::move(cb));
    });
    server.RegisterRoute(Get, "/api/v1/admin/products", HttpServer::AUTH_ADMIN_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::AdminController controller;
        controller.GetProducts(req, std::move(cb));
    });
    server.RegisterRoute(Get, "/api/v1/admin/orders", HttpServer::AUTH_ADMIN_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::AdminController controller;
        controller.GetOrders(req, std::move(cb));
    });
    server.RegisterRoute(Delete, "/api/v1/admin/products/{id}", HttpServer::AUTH_ADMIN_REQUIRED, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        std::smatch m;
        std::regex re(R"(/api/v1/admin/products/([0-9]+))");
        std::string p = req->getPath();
        if (std::regex_match(p, m, re)) {
            int64_t id = std::stoll(m[1].str());
            manimart::controller::AdminController controller;
            controller.ModerateDeleteProduct(req, std::move(cb), id);
        }
    });

    // 9. Chatbot
    server.RegisterRoute(Post, "/api/v1/chat", HttpServer::AUTH_NONE, [](const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& cb) {
        manimart::controller::ChatController controller;
        controller.SendMessage(req, std::move(cb));
    });
}

} // namespace drogon
