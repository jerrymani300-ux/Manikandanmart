#pragma once

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpFilter.h"
#include "HttpController.h"
#include "HttpServer.hpp"

#include <string>
#include <memory>
#include <regex>

// Forward declarations of controllers
namespace manimart::controller {
    class HealthController;
    class AuthController;
    class ProductController;
    class CartController;
    class OrderController;
    class ReviewController;
    class SellerController;
    class AdminController;
    class ChatController;
}

namespace drogon {

class HttpAppFramework {
public:
    static HttpAppFramework& instance() {
        static HttpAppFramework app;
        return app;
    }

    HttpAppFramework& addListener(const std::string& host, int port) {
        HttpServer::GetInstance().SetHost(host);
        HttpServer::GetInstance().SetPort(port);
        return *this;
    }

    HttpAppFramework& setThreadNum(int threads) {
        HttpServer::GetInstance().SetThreads(threads);
        return *this;
    }

    HttpAppFramework& enableSession(int /*timeout*/) {
        return *this;
    }

    HttpAppFramework& setDocumentRoot(const std::string& root) {
        HttpServer::GetInstance().SetDocumentRoot(root);
        return *this;
    }

    HttpAppFramework& setHomePage(const std::string& home) {
        HttpServer::GetInstance().SetHomePage(home);
        return *this;
    }

    HttpAppFramework& loadConfigFile(const std::string& /*config_file*/) {
        return *this;
    }

    void RegisterDefaultRoutes();

    void run() {
        RegisterDefaultRoutes();
        HttpServer::GetInstance().Start();
    }
};

inline HttpAppFramework& app() {
    return HttpAppFramework::instance();
}

} // namespace drogon
