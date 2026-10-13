#pragma once

#include "HttpRequest.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace drogon {

class HttpResponse {
public:
    virtual ~HttpResponse() = default;

    static std::shared_ptr<HttpResponse> newHttpResponse() {
        return std::make_shared<HttpResponse>();
    }

    static std::shared_ptr<HttpResponse> newHttpJsonResponse(const std::string& json_str) {
        auto res = std::make_shared<HttpResponse>();
        res->setBody(json_str);
        res->setContentTypeCode(CT_APPLICATION_JSON);
        return res;
    }

    static std::shared_ptr<HttpResponse> newNotFoundResponse() {
        auto res = std::make_shared<HttpResponse>();
        res->setStatusCode(k404NotFound);
        res->setContentTypeCode(CT_TEXT_HTML);
        res->setBody("<html><body><h1>404 Not Found</h1></body></html>");
        return res;
    }

    virtual void setStatusCode(HttpStatusCode code) { status_code_ = code; }
    [[nodiscard]] virtual HttpStatusCode getStatusCode() const { return status_code_; }

    virtual void setContentTypeCode(ContentType type) {
        content_type_ = type;
        if (type == CT_APPLICATION_JSON) {
            headers_["Content-Type"] = "application/json; charset=utf-8";
        } else if (type == CT_TEXT_HTML) {
            headers_["Content-Type"] = "text/html; charset=utf-8";
        } else {
            headers_["Content-Type"] = "text/plain; charset=utf-8";
        }
    }
    [[nodiscard]] virtual ContentType getContentTypeCode() const { return content_type_; }

    virtual void setBody(std::string b) { body_ = std::move(b); }
    [[nodiscard]] virtual const std::string& getBody() const { return body_; }

    virtual void addHeader(const std::string& key, const std::string& val) {
        headers_[key] = val;
    }
    [[nodiscard]] virtual std::string getHeader(const std::string& key) const {
        auto it = headers_.find(key);
        return it != headers_.end() ? it->second : "";
    }
    [[nodiscard]] virtual const std::unordered_map<std::string, std::string>& getHeaders() const {
        return headers_;
    }

    virtual void addCookie(const std::string& key, const std::string& val, const std::string& path = "/", bool http_only = true) {
        std::string cookie_str = key + "=" + val + "; Path=" + path;
        if (http_only) cookie_str += "; HttpOnly";
        cookie_str += "; SameSite=Lax";
        set_cookies_.push_back(cookie_str);
    }
    [[nodiscard]] virtual const std::vector<std::string>& getSetCookies() const {
        return set_cookies_;
    }

private:
    HttpStatusCode status_code_{k200OK};
    ContentType content_type_{CT_TEXT_HTML};
    std::string body_;
    std::unordered_map<std::string, std::string> headers_{
        {"Content-Type", "text/html; charset=utf-8"}
    };
    std::vector<std::string> set_cookies_;
};

using HttpResponsePtr = std::shared_ptr<HttpResponse>;

} // namespace drogon
