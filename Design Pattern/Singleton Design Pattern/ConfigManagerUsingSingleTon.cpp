#include <iostream>
#include <unordered_map>
#include <string>
#include <mutex>
#include <stdexcept>

// The hidden problem with this design
// The services (lines 67-85) reach out to the global ConfigManager directly. That means:

// You cannot test DatabaseService in isolation — it will always try to hit the real ConfigManager
// You cannot swap the config source (e.g. use a mock or an in-memory config in tests)
// The dependency is invisible — looking at DatabaseService's constructor you have no idea it needs config
// This is where Dependency Injection fixes things. 

class ConfigManager {
public:
    // The only way to get the instance
    static ConfigManager& getInstance() {
        static ConfigManager instance; // created once, destroyed at program exit
        return instance;
    }

    // Delete copy constructor and assignment — prevents cloning the singleton
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    // Thread-safe accessors for config values 
    void set(const std::string& key, const std::string& value) {
        // Lock the mutex to ensure thread safety when modifying the config
        std::lock_guard<std::mutex> lock(mutex_);
        config[key] = value;
    }

    // Thread-safe getter for config values
    std::string get(const std::string& key) const {
        // Lock the mutex to ensure thread safety when reading the config
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = config.find(key);
        if (it == config.end())
            throw std::runtime_error("Config key not found: " + key);
        return it->second;
    }

    // Thread-safe check for existence of a config key
    bool has(const std::string& key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return config.count(key) > 0;
    }

private:
    // Private constructor — simulates loading config from file/env at startup
    ConfigManager() {
        std::cout << "[ConfigManager] Initializing — loading config...\n";
        config["db_url"]       = "postgres://localhost:5432/mydb";
        config["api_key"]      = "secret-api-key-123";
        config["timeout_ms"]   = "3000";
        config["log_level"]    = "INFO";
    }

    std::unordered_map<std::string, std::string> config;
    mutable std::mutex mutex_; // mutable so const methods can lock it
};


// --- Services that consume config ---

class DatabaseService {
public:
    void connect() {
        // Reaches out to the global singleton directly
        std::string url = ConfigManager::getInstance().get("db_url");
        std::cout << "[DatabaseService] Connecting to: " << url << "\n";
    }
};

class AuthService {
public:
    void authenticate() {
        std::string key = ConfigManager::getInstance().get("api_key");
        std::cout << "[AuthService] Using API key: " << key << "\n";
    }
};

class EmailService {
public:
    void send() {
        std::string timeout = ConfigManager::getInstance().get("timeout_ms");
        std::cout << "[EmailService] Sending email with timeout: " << timeout << "ms\n";
    }
};


int main() {
    std::cout << "=== Singleton Config Manager ===\n\n";

    // Prove that getInstance() always returns the same object
    ConfigManager& c1 = ConfigManager::getInstance();
    ConfigManager& c2 = ConfigManager::getInstance();
    std::cout << "Same instance? " << (&c1 == &c2 ? "YES" : "NO") << "\n\n";

    // Services use config — constructor runs only once despite multiple getInstance() calls
    DatabaseService db;
    db.connect();

    AuthService auth;
    auth.authenticate();

    EmailService email;
    email.send();

    // Runtime override
    std::cout << "\n[main] Overriding log_level to DEBUG\n";
    ConfigManager::getInstance().set("log_level", "DEBUG");
    std::cout << "[main] log_level is now: " << ConfigManager::getInstance().get("log_level") << "\n";

    return 0;
}
