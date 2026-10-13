#pragma once

#include "HttpRequest.h"
#include "HttpResponse.h"
#include <spdlog/spdlog.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <regex>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <queue>
#include <condition_variable>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #ifdef SendMessage
        #undef SendMessage
    #endif
    typedef SOCKET socket_t;
    #define INVALID_SOCK INVALID_SOCKET
    #define CLOSE_SOCK closesocket
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int socket_t;
    #define INVALID_SOCK (-1)
    #define CLOSE_SOCK close
#endif

namespace drogon {

class HttpServer {
public:
    using HandlerFunc = std::function<void(const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&&)>;

    enum RouteAuth {
        AUTH_NONE,
        AUTH_REQUIRED,
        AUTH_SELLER_REQUIRED,
        AUTH_ADMIN_REQUIRED
    };

    struct Route {
        HttpMethod method;
        std::string path_pattern;
        std::regex regex;
        RouteAuth auth;
        HandlerFunc handler;
    };

    static HttpServer& GetInstance() {
        static HttpServer instance;
        return instance;
    }

    void SetHost(std::string host) { host_ = std::move(host); }
    void SetPort(int port) { port_ = port; }
    void SetThreads(int threads) { threads_ = threads > 0 ? threads : 4; }
    void SetDocumentRoot(std::string root) { document_root_ = std::move(root); }
    void SetHomePage(std::string home) { home_page_ = std::move(home); }

    void RegisterRoute(HttpMethod method, const std::string& path, RouteAuth auth, HandlerFunc handler) {
        std::string regex_str = "^" + std::regex_replace(path, std::regex(R"(\{([0-9a-zA-Z_]+)\})"), R"(([0-9]+))") + "$";
        Route r{method, path, std::regex(regex_str), auth, std::move(handler)};
        routes_.push_back(std::move(r));
    }

    std::shared_ptr<Session> GetOrCreateSession(const std::string& session_id) {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        if (!session_id.empty()) {
            auto it = sessions_.find(session_id);
            if (it != sessions_.end()) {
                return it->second;
            }
        }
        auto new_session = std::make_shared<Session>();
        sessions_[new_session->sessionId()] = new_session;
        return new_session;
    }

    void Start() {
        running_ = true;

#ifdef _WIN32
        WSADATA wsaData;
        int wsa_res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (wsa_res != 0) {
            spdlog::error("WSAStartup failed with error: {}", wsa_res);
            return;
        }
#endif

        server_sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_sock_ == INVALID_SOCK) {
            spdlog::error("Failed to create socket");
            return;
        }

        int opt = 1;
#ifdef _WIN32
        setsockopt(server_sock_, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
        setsockopt(server_sock_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

        sockaddr_in server_addr{};
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(static_cast<uint16_t>(port_));
        if (host_ == "0.0.0.0" || host_.empty()) {
            server_addr.sin_addr.s_addr = INADDR_ANY;
        } else {
            inet_pton(AF_INET, host_.c_str(), &server_addr.sin_addr);
        }

        if (bind(server_sock_, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
            spdlog::error("Failed to bind socket to {}:{}", host_, port_);
            CLOSE_SOCK(server_sock_);
            return;
        }

        if (listen(server_sock_, SOMAXCONN) != 0) {
            spdlog::error("Failed to listen on socket");
            CLOSE_SOCK(server_sock_);
            return;
        }

        spdlog::info("ManiMart embedded HTTP server listening on http://{}:{}", host_, port_);

        // Start worker thread pool
        for (int i = 0; i < threads_; ++i) {
            workers_.emplace_back(&HttpServer::WorkerLoop, this);
        }

        // Accept loop
        while (running_) {
            sockaddr_in client_addr{};
#ifdef _WIN32
            int client_len = sizeof(client_addr);
#else
            socklen_t client_len = sizeof(client_addr);
#endif
            socket_t client_sock = accept(server_sock_, (struct sockaddr*)&client_addr, &client_len);
            if (client_sock == INVALID_SOCK) {
                if (!running_) break;
                continue;
            }

#ifdef _WIN32
            DWORD sock_timeout = 5000;
            setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&sock_timeout, sizeof(sock_timeout));
            setsockopt(client_sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&sock_timeout, sizeof(sock_timeout));
#else
            struct timeval tv{5, 0};
            setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
            setsockopt(client_sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));
#endif

            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                client_queue_.push({client_sock, std::string(client_ip)});
            }
            queue_cv_.notify_one();
        }

        // Cleanup
        for (auto& w : workers_) {
            if (w.joinable()) w.join();
        }

#ifdef _WIN32
        WSACleanup();
#endif
    }

    void Stop() {
        running_ = false;
        if (server_sock_ != INVALID_SOCK) {
            CLOSE_SOCK(server_sock_);
            server_sock_ = INVALID_SOCK;
        }
        queue_cv_.notify_all();
    }

private:
    struct ClientTask {
        socket_t sock;
        std::string ip;
    };

    void WorkerLoop() {
        while (running_) {
            ClientTask task{INVALID_SOCK, ""};
            {
                std::unique_lock<std::mutex> lock(queue_mutex_);
                queue_cv_.wait(lock, [this]() {
                    return !running_ || !client_queue_.empty();
                });

                if (!running_ && client_queue_.empty()) return;

                task = client_queue_.front();
                client_queue_.pop();
            }

            if (task.sock != INVALID_SOCK) {
                HandleClient(task.sock, task.ip);
                CLOSE_SOCK(task.sock);
            }
        }
    }

    void HandleClient(socket_t sock, const std::string& client_ip) {
        std::string raw_request;
        char buffer[4096];
        int bytes_read = 0;

        // Read headers first
        size_t header_end_pos = std::string::npos;
        while ((bytes_read = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[bytes_read] = '\0';
            raw_request.append(buffer, static_cast<size_t>(bytes_read));
            header_end_pos = raw_request.find("\r\n\r\n");
            if (header_end_pos != std::string::npos) {
                break;
            }
            if (raw_request.size() > 65536) break; // header limit
        }

        if (raw_request.empty()) return;

        // Parse Request Line and Headers
        std::istringstream stream(raw_request);
        std::string request_line;
        std::getline(stream, request_line);
        if (!request_line.empty() && request_line.back() == '\r') {
            request_line.pop_back();
        }

        std::istringstream req_line_stream(request_line);
        std::string method_str, full_path, http_ver;
        req_line_stream >> method_str >> full_path >> http_ver;

        if (method_str.empty() || full_path.empty()) return;

        HttpMethod method = Get;
        if (method_str == "POST") method = Post;
        else if (method_str == "PUT") method = Put;
        else if (method_str == "DELETE") method = Delete;
        else if (method_str == "OPTIONS") method = Options;

        // Parse URL and Query Parameters
        std::string path = full_path;
        std::string query_string;
        size_t q_pos = full_path.find('?');
        if (q_pos != std::string::npos) {
            path = full_path.substr(0, q_pos);
            query_string = full_path.substr(q_pos + 1);
        }

        auto req = std::make_shared<HttpRequest>();
        req->setMethod(method);
        req->setPath(path);
        PeerAddr peer;
        peer.ip = client_ip;
        req->setPeerAddr(peer);

        // Parse query params
        if (!query_string.empty()) {
            std::istringstream q_stream(query_string);
            std::string pair;
            while (std::getline(q_stream, pair, '&')) {
                size_t eq_pos = pair.find('=');
                if (eq_pos != std::string::npos) {
                    std::string k = UrlDecode(pair.substr(0, eq_pos));
                    std::string v = UrlDecode(pair.substr(eq_pos + 1));
                    req->setParameter(k, v);
                } else if (!pair.empty()) {
                    req->setParameter(UrlDecode(pair), "");
                }
            }
        }

        // Parse headers
        std::string header_line;
        size_t content_length = 0;
        std::string cookie_header;
        while (std::getline(stream, header_line) && header_line != "\r" && !header_line.empty()) {
            if (header_line.back() == '\r') header_line.pop_back();
            size_t colon_pos = header_line.find(':');
            if (colon_pos != std::string::npos) {
                std::string k = header_line.substr(0, colon_pos);
                std::string v = header_line.substr(colon_pos + 1);
                while (!v.empty() && v.front() == ' ') v.erase(v.begin());
                req->setHeader(k, v);

                std::string lower_k = ToLower(k);
                if (lower_k == "content-length") {
                    try { content_length = std::stoul(v); } catch (...) {}
                } else if (lower_k == "cookie") {
                    cookie_header = v;
                }
            }
        }

        // Parse cookies
        std::string session_id_from_cookie;
        if (!cookie_header.empty()) {
            std::istringstream c_stream(cookie_header);
            std::string c_pair;
            while (std::getline(c_stream, c_pair, ';')) {
                while (!c_pair.empty() && c_pair.front() == ' ') c_pair.erase(c_pair.begin());
                size_t eq_pos = c_pair.find('=');
                if (eq_pos != std::string::npos) {
                    std::string k = c_pair.substr(0, eq_pos);
                    std::string v = c_pair.substr(eq_pos + 1);
                    req->setCookie(k, v);
                    if (k == "manimart_session") {
                        session_id_from_cookie = v;
                    }
                }
            }
        }

        // Also check Authorization or X-Session-ID headers for session identification
        if (session_id_from_cookie.empty()) {
            std::string auth_h = req->getHeader("Authorization");
            if (auth_h.rfind("Bearer ", 0) == 0) {
                session_id_from_cookie = auth_h.substr(7);
            }
        }
        if (session_id_from_cookie.empty()) {
            session_id_from_cookie = req->getHeader("X-Session-ID");
        }

        // Attach session
        auto session = GetOrCreateSession(session_id_from_cookie);
        req->setSession(session);

        // Read remaining body if content-length > 0
        size_t body_start_pos = (header_end_pos != std::string::npos) ? header_end_pos + 4 : raw_request.size();
        std::string body;
        if (body_start_pos < raw_request.size()) {
            body = raw_request.substr(body_start_pos);
        }

        while (body.size() < content_length) {
            bytes_read = recv(sock, buffer, sizeof(buffer) - 1, 0);
            if (bytes_read <= 0) break;
            buffer[bytes_read] = '\0';
            body.append(buffer, static_cast<size_t>(bytes_read));
        }
        req->setBody(body);

        // Handle CORS Preflight
        if (method == Options) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k200OK);
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With, X-Session-ID, X-Request-ID");
            resp->addHeader("Access-Control-Allow-Credentials", "true");
            SendResponse(sock, req, resp);
            return;
        }

        // Route Matching
        bool matched = false;
        for (const auto& r : routes_) {
            if (r.method == method && std::regex_match(path, r.regex)) {
                matched = true;

                // Check Role & Auth Constraints
                if (r.auth == AUTH_REQUIRED) {
                    if (!session || !session->find("user_id")) {
                        auto err_resp = HttpResponse::newHttpJsonResponse(
                            R"({"error":"AUTHENTICATION_ERROR","message":"Authentication required. Please log in to proceed."})"
                        );
                        err_resp->setStatusCode(k401Unauthorized);
                        SendResponse(sock, req, err_resp);
                        return;
                    }
                } else if (r.auth == AUTH_SELLER_REQUIRED) {
                    if (!session || !session->find("user_id")) {
                        auto err_resp = HttpResponse::newHttpJsonResponse(
                            R"({"error":"AUTHENTICATION_ERROR","message":"Authentication required. Please log in to proceed."})"
                        );
                        err_resp->setStatusCode(k401Unauthorized);
                        SendResponse(sock, req, err_resp);
                        return;
                    }
                    std::string role = session->get<std::string>("role");
                    if (role != "SELLER") {
                        auto err_resp = HttpResponse::newHttpJsonResponse(
                            R"({"error":"FORBIDDEN","message":"Access restricted to seller accounts only."})"
                        );
                        err_resp->setStatusCode(k403Forbidden);
                        SendResponse(sock, req, err_resp);
                        return;
                    }
                } else if (r.auth == AUTH_ADMIN_REQUIRED) {
                    if (!session || !session->find("user_id")) {
                        auto err_resp = HttpResponse::newHttpJsonResponse(
                            R"({"error":"AUTHENTICATION_ERROR","message":"Authentication required. Please log in to proceed."})"
                        );
                        err_resp->setStatusCode(k401Unauthorized);
                        SendResponse(sock, req, err_resp);
                        return;
                    }
                    std::string role = session->get<std::string>("role");
                    if (role != "ADMIN") {
                        auto err_resp = HttpResponse::newHttpJsonResponse(
                            R"({"error":"FORBIDDEN","message":"Access restricted to admin accounts only."})"
                        );
                        err_resp->setStatusCode(k403Forbidden);
                        SendResponse(sock, req, err_resp);
                        return;
                    }
                }

                // Execute Controller Handler
                r.handler(req, [this, sock, req](const HttpResponsePtr& resp) {
                    SendResponse(sock, req, resp);
                });
                return;
            }
        }

        // Static File Serving
        if (!matched) {
            if (path.rfind("/api/", 0) == 0) {
                auto err_resp = HttpResponse::newHttpJsonResponse(
                    R"({"error":"NOT_FOUND","message":"API route not found"})"
                );
                err_resp->setStatusCode(k404NotFound);
                SendResponse(sock, req, err_resp);
                return;
            }

            ServeStaticFile(sock, req, path);
        }
    }

    void ServeStaticFile(socket_t sock, const HttpRequestPtr& req, const std::string& path) {
        std::string relative_path = path;
        if (relative_path == "/" || relative_path.empty()) {
            relative_path = "/" + home_page_;
        }

        // Prevent path traversal
        if (relative_path.find("..") != std::string::npos) {
            auto resp = HttpResponse::newNotFoundResponse();
            SendResponse(sock, req, resp);
            return;
        }

        std::filesystem::path file_path = std::filesystem::path(document_root_) / relative_path.substr(1);

        if (!std::filesystem::exists(file_path) || std::filesystem::is_directory(file_path)) {
            // Try fallback to index.html if not found
            std::filesystem::path fallback_path = std::filesystem::path(document_root_) / home_page_;
            if (std::filesystem::exists(fallback_path) && !std::filesystem::is_directory(fallback_path)) {
                file_path = fallback_path;
            } else {
                auto resp = HttpResponse::newNotFoundResponse();
                SendResponse(sock, req, resp);
                return;
            }
        }

        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            auto resp = HttpResponse::newNotFoundResponse();
            SendResponse(sock, req, resp);
            return;
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        std::string content = ss.str();

        std::string ext = file_path.extension().string();
        std::string content_type = GetMimeType(ext);

        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k200OK);
        resp->setBody(content);
        resp->addHeader("Content-Type", content_type);
        SendResponse(sock, req, resp);
    }

    void SendResponse(socket_t sock, const HttpRequestPtr& req, const HttpResponsePtr& resp) {
        if (!resp) return;

        std::ostringstream ss;
        int status_code = static_cast<int>(resp->getStatusCode());
        std::string status_text = GetStatusText(resp->getStatusCode());

        ss << "HTTP/1.1 " << status_code << " " << status_text << "\r\n";
        ss << "Content-Length: " << resp->getBody().size() << "\r\n";
        ss << "Access-Control-Allow-Origin: *\r\n";
        ss << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
        ss << "Access-Control-Allow-Headers: Content-Type, Authorization, X-Requested-With, X-Session-ID, X-Request-ID\r\n";
        ss << "Access-Control-Allow-Credentials: true\r\n";

        // Set session cookie if active
        if (req && req->session()) {
            ss << "Set-Cookie: manimart_session=" << req->session()->sessionId() << "; Path=/; HttpOnly; SameSite=Lax\r\n";
        }

        for (const auto& [k, v] : resp->getHeaders()) {
            ss << k << ": " << v << "\r\n";
        }

        for (const auto& cookie : resp->getSetCookies()) {
            ss << "Set-Cookie: " << cookie << "\r\n";
        }

        ss << "Connection: close\r\n\r\n";
        ss << resp->getBody();

        std::string full_response = ss.str();
        size_t total_sent = 0;
        const char* buf = full_response.c_str();
        size_t to_send = full_response.size();
        while (total_sent < to_send) {
            int sent = send(sock, buf + total_sent, static_cast<int>(to_send - total_sent), 0);
            if (sent <= 0) break;
            total_sent += static_cast<size_t>(sent);
        }
#ifdef _WIN32
        shutdown(sock, SD_SEND);
#else
        shutdown(sock, SHUT_WR);
#endif
    }

    static std::string GetMimeType(const std::string& ext) {
        if (ext == ".html" || ext == ".htm") return "text/html; charset=utf-8";
        if (ext == ".css") return "text/css; charset=utf-8";
        if (ext == ".js") return "application/javascript; charset=utf-8";
        if (ext == ".json") return "application/json";
        if (ext == ".png") return "image/png";
        if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
        if (ext == ".gif") return "image/gif";
        if (ext == ".svg") return "image/svg+xml";
        if (ext == ".ico") return "image/x-icon";
        return "application/octet-stream";
    }

    static std::string GetStatusText(HttpStatusCode code) {
        switch (code) {
            case k200OK: return "OK";
            case k201Created: return "Created";
            case k204NoContent: return "No Content";
            case k400BadRequest: return "Bad Request";
            case k401Unauthorized: return "Unauthorized";
            case k403Forbidden: return "Forbidden";
            case k404NotFound: return "Not Found";
            case k409Conflict: return "Conflict";
            case k429TooManyRequests: return "Too Many Requests";
            case k500InternalServerError: return "Internal Server Error";
            case k503ServiceUnavailable: return "Service Unavailable";
            default: return "OK";
        }
    }

    static std::string ToLower(std::string str) {
        for (char& c : str) c = static_cast<char>(::tolower(c));
        return str;
    }

    static std::string UrlDecode(const std::string& in) {
        std::string out;
        out.reserve(in.size());
        for (size_t i = 0; i < in.size(); ++i) {
            if (in[i] == '%') {
                if (i + 2 < in.size()) {
                    int hex_val;
                    std::istringstream hex_stream(in.substr(i + 1, 2));
                    if (hex_stream >> std::hex >> hex_val) {
                        out += static_cast<char>(hex_val);
                        i += 2;
                    } else {
                        out += '%';
                    }
                } else {
                    out += '%';
                }
            } else if (in[i] == '+') {
                out += ' ';
            } else {
                out += in[i];
            }
        }
        return out;
    }

    std::string host_{"0.0.0.0"};
    int port_{8080};
    int threads_{4};
    std::string document_root_{"./frontend"};
    std::string home_page_{"index.html"};
    std::atomic<bool> running_{false};
    socket_t server_sock_{INVALID_SOCK};

    std::vector<Route> routes_;
    std::vector<std::thread> workers_;
    std::queue<ClientTask> client_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;

    std::unordered_map<std::string, std::shared_ptr<Session>> sessions_;
    std::mutex sessions_mutex_;
};

} // namespace drogon
