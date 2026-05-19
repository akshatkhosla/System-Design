#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>

// ─── Step 1: Abstract interface ───────────────────────────────────────────────
// Services depend on this, not on the concrete class.
// This is the contract — any config provider must satisfy it.
class IConfigManager {
public:
    virtual std::string get(const std::string& key) const = 0;
    virtual bool has(const std::string& key) const = 0;
    virtual ~IConfigManager() = default;
};


// ─── Step 2: Real implementation ──────────────────────────────────────────────
// No singleton mechanics needed — the caller controls lifetime.
class ConfigManager : public IConfigManager {
public:
    ConfigManager() {
        std::cout << "[ConfigManager] Loading config from file/env...\n";
        config_["db_url"]     = "postgres://localhost:5432/mydb";
        config_["api_key"]    = "secret-api-key-123";
        config_["timeout_ms"] = "3000";
        config_["log_level"]  = "INFO";
    }

    std::string get(const std::string& key) const override {
        auto it = config_.find(key);
        if (it == config_.end())
            throw std::runtime_error("Config key not found: " + key);
        return it->second;
    }

    bool has(const std::string& key) const override {
        return config_.count(key) > 0;
    }

private:
    std::unordered_map<std::string, std::string> config_;
};


// ─── Step 3: Mock for testing ──────────────────────────────────────────────────
// Same interface, no file I/O, values controlled by the test.
class MockConfigManager : public IConfigManager {
public:
    void set(const std::string& key, const std::string& value) {
        config_[key] = value;
    }

    std::string get(const std::string& key) const override {
        auto it = config_.find(key);
        if (it == config_.end())
            throw std::runtime_error("Mock key not found: " + key);
        return it->second;
    }

    bool has(const std::string& key) const override {
        return config_.count(key) > 0;
    }

private:
    std::unordered_map<std::string, std::string> config_;
};


// ─── Step 4: Services receive config via constructor ──────────────────────────
// The dependency is explicit and visible — no hidden global access.

class DatabaseService {
public:
    // Constructor injection — the service explicitly states it needs a config manager
    explicit DatabaseService(IConfigManager& config) : config_(config) {}

    void connect() {
        std::string url = config_.get("db_url");
        std::cout << "[DatabaseService] Connecting to: " << url << "\n";
    }

private:
    IConfigManager& config_;
};

class AuthService {
public:
    explicit AuthService(IConfigManager& config) : config_(config) {}

    void authenticate() {
        std::string key = config_.get("api_key");
        std::cout << "[AuthService] Using API key: " << key << "\n";
    }

private:
    IConfigManager& config_;
};

class EmailService {
public:
    explicit EmailService(IConfigManager& config) : config_(config) {}

    void send() {
        std::string timeout = config_.get("timeout_ms");
        std::cout << "[EmailService] Sending with timeout: " << timeout << "ms\n";
    }

private:
    IConfigManager& config_;
};


// ─── Production wiring ────────────────────────────────────────────────────────
void runProduction() {
    std::cout << "=== PRODUCTION ===\n";
    ConfigManager config;                  // created once, passed everywhere

    DatabaseService db(config);
    AuthService     auth(config);
    EmailService    email(config);

    db.connect();
    auth.authenticate();
    email.send();
}

// ─── Test wiring ──────────────────────────────────────────────────────────────
// No real file reads. No global state. Fully isolated.
void runTests() {
    std::cout << "\n=== TESTS (using MockConfigManager) ===\n";

    MockConfigManager mock;
    mock.set("db_url",     "postgres://test-host:5432/testdb");
    mock.set("api_key",    "test-key-xyz");
    mock.set("timeout_ms", "100");

    DatabaseService db(mock);
    AuthService     auth(mock);
    EmailService    email(mock);

    db.connect();
    auth.authenticate();
    email.send();
}

int main() {
    runProduction();
    runTests();
    return 0;
}
