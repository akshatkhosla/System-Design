#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>

// Forward declaration so HTTPRequest can grant builder access via a clean constructor
class HTTPRequestBuilder;

// ─── Product ───────────────────────────────────────────────────────────────────
// HTTPRequest is immutable once built — all fields are set exactly once via the builder.
// Private constructor prevents direct construction; only HTTPRequestBuilder can call it.
class HTTPRequest {
public:
    void display() const {
        std::cout << "Method:  " << method_ << "\n";
        std::cout << "URL:     " << url_ << "\n";
        std::cout << "Headers:\n";
        for (const auto& [key, value] : headers_)
            std::cout << "  " << key << ": " << value << "\n";
        std::cout << "Body:    " << body_ << "\n";
    }

private:
    // Only the builder can call this — no friend needed, just a private constructor
    // that the builder reaches via a factory method pattern on itself.
    HTTPRequest(std::string method,
                std::string url,
                std::unordered_map<std::string, std::string> headers,
                std::string body)
        : method_(std::move(method))
        , url_(std::move(url))
        , headers_(std::move(headers))
        , body_(std::move(body)) {}

    friend class HTTPRequestBuilder;

    std::string method_;
    std::string url_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;
};


// ─── Builder ───────────────────────────────────────────────────────────────────
// Constructs an HTTPRequest step by step.
// Each setter returns *this so calls can be chained fluently.
// build() validates required fields and hands off ownership via move.
class HTTPRequestBuilder {
public:
    HTTPRequestBuilder& withMethod(const std::string& method) {
        this->method_ = method;
        return *this;
    }

     
    HTTPRequestBuilder& withURL(const std::string& url) {
        this->url_ = url;
        return *this;
    }

    HTTPRequestBuilder& withHeader(const std::string& key, const std::string& value) {
        this->headers_[key] = value;
        return *this;
    }

    HTTPRequestBuilder& withBody(const std::string& body) {
        this->body_ = body;
        return *this;
    }

    // Validates required fields, then constructs and moves the HTTPRequest out.
    // After build(), this builder should not be reused.
    HTTPRequest build() {
        if (method_.empty()) throw std::logic_error("HTTPRequest requires a method");
        if (url_.empty())    throw std::logic_error("HTTPRequest requires a URL");

        return HTTPRequest(
            std::move(method_),
            std::move(url_),
            std::move(headers_),
            std::move(body_)
        );
    }

private:
    std::string method_;
    std::string url_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;
};


int main() {
    // Fluent chain — each call returns the builder, build() returns the finished object
    HTTPRequest request = HTTPRequestBuilder()
        .withMethod("POST")
        .withURL("https://example.com/api/data")
        .withHeader("Content-Type", "application/json")
        .withHeader("Authorization", "Bearer token-xyz")
        .withBody(R"({"key": "value"})")
        .build();

    request.display();

    // Validation check — missing method should throw
    std::cout << "\n=== Missing method (expect exception) ===\n";
    try {
        HTTPRequest bad = HTTPRequestBuilder()
            .withURL("https://example.com")
            .build();
    } catch (const std::logic_error& e) {
        std::cout << "Caught: " << e.what() << "\n";
    }

    return 0;
}
