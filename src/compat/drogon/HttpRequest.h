#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <any>
#include <random>
#include <sstream>

namespace drogon {

enum HttpMethod {
    Get,
    Post,
    Put,
    Delete,
    Options
};

enum ContentType {
    CT_APPLICATION_JSON,
    CT_TEXT_HTML,
    CT_TEXT_PLAIN
};

enum HttpStatusCode {
    k200OK = 200,
    k201Created = 201,
    k204NoContent = 204,
    k400BadRequest = 400,
    k401Unauthorized = 401,
    k403Forbidden = 403,
    k404NotFound = 404,
    k409Conflict = 409,
    k429TooManyRequests = 429,
    k500InternalServerError = 500,
    k503ServiceUnavailable = 503
};

class Session {
public:
    Session() {
        session_id_ = GenerateId();
    }

    explicit Session(std::string id) : session_id_(std::move(id)) {}

    virtual ~Session() = default;

    virtual bool find(const std::string& key) const {
        return data_.find(key) != data_.end();
    }

    virtual void insert(const std::string& key, const std::any& val) {
        data_[key] = val;
    }

    virtual void erase(const std::string& key) {
        data_.erase(key);
    }

    virtual std::string sessionId() const {
        return session_id_;
    }

    template <typename T>
    T get(const std::string& key) const {
        auto it = data_.find(key);
        if (it != data_.end()) {
            return std::any_cast<T>(it->second);
        }
        return T{};
    }

private:
    static std::string GenerateId() {
        static std::random_device rd;
        static std::mt19937_64 gen(rd());
        static std::uniform_int_distribution<uint64_t> dis;
        std::stringstream ss;
        ss << "sess_" << std::hex << dis(gen) << dis(gen);
        return ss.str();
    }

    std::string session_id_;
    std::unordered_map<std::string, std::any> data_;
};

using SessionPtr = std::shared_ptr<Session>;

class Attributes {
public:
    void insert(const std::string& key, const std::any& val) { attrs_[key] = val; }
    bool find(const std::string& key) const { return attrs_.find(key) != attrs_.end(); }

    template <typename T>
    T get(const std::string& key) const {
        auto it = attrs_.find(key);
        if (it != attrs_.end()) {
            return std::any_cast<T>(it->second);
        }
        return T{};
    }

private:
    std::unordered_map<std::string, std::any> attrs_;
};

using AttributesPtr = std::shared_ptr<Attributes>;

struct PeerAddr {
    std::string ip{"127.0.0.1"};
    std::string toIp() const { return ip; }
};

class HttpRequest {
public:
    virtual ~HttpRequest() = default;

    [[nodiscard]] virtual const std::string& body() const { return body_; }
    virtual void setBody(std::string b) { body_ = std::move(b); }

    [[nodiscard]] virtual std::string getParameter(const std::string& key) const {
        auto it = params_.find(key);
        return it != params_.end() ? it->second : "";
    }
    virtual void setParameter(const std::string& k, const std::string& v) { params_[k] = v; }
    [[nodiscard]] virtual const std::unordered_map<std::string, std::string>& getParameters() const { return params_; }

    [[nodiscard]] virtual std::string getHeader(const std::string& key) const {
        std::string lower_k = key;
        for (char& c : lower_k) c = static_cast<char>(::tolower(c));
        for (const auto& [k, v] : headers_) {
            std::string cur_k = k;
            for (char& c : cur_k) c = static_cast<char>(::tolower(c));
            if (cur_k == lower_k) return v;
        }
        return "";
    }
    virtual void setHeader(const std::string& k, const std::string& v) { headers_[k] = v; }
    [[nodiscard]] virtual const std::unordered_map<std::string, std::string>& getHeaders() const { return headers_; }

    [[nodiscard]] virtual std::string getCookie(const std::string& key) const {
        auto it = cookies_.find(key);
        return it != cookies_.end() ? it->second : "";
    }
    virtual void setCookie(const std::string& k, const std::string& v) { cookies_[k] = v; }

    [[nodiscard]] virtual HttpMethod method() const { return method_; }
    virtual void setMethod(HttpMethod m) { method_ = m; }
    [[nodiscard]] virtual std::string getMethodString() const {
        switch (method_) {
            case Get: return "GET";
            case Post: return "POST";
            case Put: return "PUT";
            case Delete: return "DELETE";
            case Options: return "OPTIONS";
        }
        return "GET";
    }

    [[nodiscard]] virtual std::string getPath() const { return path_; }
    virtual void setPath(std::string p) { path_ = std::move(p); }

    [[nodiscard]] virtual SessionPtr session() const { return session_; }
    virtual void setSession(SessionPtr s) { session_ = std::move(s); }

    [[nodiscard]] virtual AttributesPtr attributes() const { return attributes_; }
    [[nodiscard]] virtual PeerAddr getPeerAddr() const { return peer_addr_; }
    virtual void setPeerAddr(const PeerAddr& addr) { peer_addr_ = addr; }

private:
    std::string body_;
    std::string path_{"/"};
    HttpMethod method_{Get};
    std::unordered_map<std::string, std::string> params_;
    std::unordered_map<std::string, std::string> headers_;
    std::unordered_map<std::string, std::string> cookies_;
    SessionPtr session_{std::make_shared<Session>()};
    AttributesPtr attributes_{std::make_shared<Attributes>()};
    PeerAddr peer_addr_{};
};

using HttpRequestPtr = std::shared_ptr<HttpRequest>;

} // namespace drogon
