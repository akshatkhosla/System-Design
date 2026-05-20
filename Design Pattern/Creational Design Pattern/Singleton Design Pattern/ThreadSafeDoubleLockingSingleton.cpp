#include<iostream>
#include<mutex>
using namespace std;

// Thread-Safe Double-Checked Locking Singleton Implementation
// This implementation ensures that the singleton instance is created in a thread-safe manner while minimizing locking overhead
class Singleton {
    private: 
        static Singleton* instance; // Static member to hold the single instance
        static std::mutex mutex; // Mutex for thread safety
    Singleton() {
        cout << "Singleton instance created!" << endl;
    }
    public:
    static Singleton* getInstance() {
        if (!instance) { // First check (without locking)
            lock_guard<std::mutex> lock(mutex); // Lock for thread safety
            if (!instance) { // Second check (with locking)
                instance = new Singleton();
            }
        }
        return instance;
    }
};
Singleton* Singleton::instance = nullptr; // Initialize the static member variable to nullptr
std::mutex Singleton::mutex; // Initialize the mutex

int main() {
    Singleton* s1 = Singleton::getInstance(); // Creates the instance
    Singleton* s2 = Singleton::getInstance(); // Returns the existing instance

    if(s1 == s2) {
        cout << "Both instances are the same!" << endl;
    } else {
        cout << "Instances are different!" << endl;
    }
    return 0;
}