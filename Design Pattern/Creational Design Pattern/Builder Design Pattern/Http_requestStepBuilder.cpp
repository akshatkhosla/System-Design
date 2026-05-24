#include <iostream>
#include <string>
#include <unordered_map>
#include <stdexcept>

class HTTPRequest;
class HTTPRequestStepBuilder;

// ─── Product ───────────────────────────────────────────────────────────────────
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
    HTTPRequest(std::string method,
                std::string url,
                std::unordered_map<std::string, std::string> headers,
                std::string body)
        : method_(std::move(method))
        , url_(std::move(url))
        , headers_(std::move(headers))
        , body_(std::move(body)) {}

    friend class HTTPRequestStepBuilder;

    std::string method_;
    std::string url_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;
};


// ─── Step Interfaces ───────────────────────────────────────────────────────────
// Each interface exposes ONLY what is valid at that stage of construction.
// The compiler makes it impossible to skip a required step or call out of order.

// Forward declare the builder so interfaces can return a reference to it
class HTTPRequestStepBuilder;

// Step 1 — only withMethod() is callable. Nothing else exists yet.
struct IMethodStep {
    virtual HTTPRequestStepBuilder& withMethod(const std::string& method) = 0;
    virtual ~IMethodStep() = default;
};

// Step 2 — only withURL() is callable. Method is already set.
struct IUrlStep {
    virtual HTTPRequestStepBuilder& withURL(const std::string& url) = 0;
    virtual ~IUrlStep() = default;
};

// Step 3 — optional: add headers, add body, or call build().
// This is the only step where build() is reachable.
struct IOptionalStep {
    virtual HTTPRequestStepBuilder& withHeader(const std::string& key, const std::string& value) = 0;
    virtual HTTPRequestStepBuilder& withBody(const std::string& body) = 0;
    virtual HTTPRequest build() = 0;
    virtual ~IOptionalStep() = default;
};


// ─── Step Builder ──────────────────────────────────────────────────────────────
// Implements all three step interfaces. The return type at each step restricts
// what the caller can do next — enforced entirely by the type system, not runtime checks.
class HTTPRequestStepBuilder : public IMethodStep, public IUrlStep, public IOptionalStep {
public:
    // Entry point — returns IMethodStep so only withMethod() is visible to the caller
    static IMethodStep& create() {
        // Heap-allocated so the reference stays alive across the chain.
        // In production you'd use a unique_ptr or arena; here it's intentionally simple.
        return *(new HTTPRequestStepBuilder());
    }

    // Step 1: set method → unlocks Step 2 by returning IUrlStep
    HTTPRequestStepBuilder& withMethod(const std::string& method) override {
        method_ = method;
        return *this;    // returns *this, but typed as HTTPRequestStepBuilder&
    }

    // Step 2: set URL → unlocks Step 3 by returning IOptionalStep
    HTTPRequestStepBuilder& withURL(const std::string& url) override {
        url_ = url;
        return *this;
    }

    // Step 3 (optional): headers, body, or build — all reachable via IOptionalStep
    HTTPRequestStepBuilder& withHeader(const std::string& key, const std::string& value) override {
        headers_[key] = value;
        return *this;
    }

    HTTPRequestStepBuilder& withBody(const std::string& body) override {
        body_ = body;
        return *this;
    }

    // build() is ONLY reachable via IOptionalStep — only after method + URL are set.
    // No runtime validation needed: the type system already guaranteed them.
    HTTPRequest build() override {
        HTTPRequest req(
            std::move(method_),
            std::move(url_),
            std::move(headers_),
            std::move(body_)
        );
        delete this; // clean up heap allocation from create()
        return req;
    }

private:
    HTTPRequestStepBuilder() = default; // only create() can start the chain

    std::string method_;
    std::string url_;
    std::unordered_map<std::string, std::string> headers_;
    std::string body_;
};


int main() {
    std::cout << "=== Step Builder — enforced order ===\n\n";

    // The type at each dot is different — the compiler only shows valid next steps:
    //   create()       → IMethodStep  (only withMethod available)
    //   withMethod()   → IUrlStep     (only withURL available)
    //   withURL()      → IOptionalStep (withHeader / withBody / build available)
    HTTPRequest request = HTTPRequestStepBuilder::create()
        .withMethod("POST")
        .withURL("https://example.com/api/data")
        .withHeader("Content-Type", "application/json")
        .withHeader("Authorization", "Bearer token-xyz")
        .withBody(R"({"key": "value"})")
        .build();

    request.display();

    // ── What the step builder PREVENTS (these don't compile) ──────────────────
    //
    // Skipping method:
    //   HTTPRequestStepBuilder::create()
    //       .withURL("...")   // ERROR: IMethodStep has no withURL()
    //
    // Skipping URL:
    //   HTTPRequestStepBuilder::create()
    //       .withMethod("GET")
    //       .build()          // ERROR: IUrlStep has no build()
    //
    // Calling build() before method+URL:
    //   HTTPRequestStepBuilder::create()
    //       .build()          // ERROR: IMethodStep has no build()

    return 0;
}
