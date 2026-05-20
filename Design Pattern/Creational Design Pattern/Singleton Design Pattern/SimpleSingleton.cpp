#include<iostream>
#include<string>
using namespace std;    

// Singleton Design Pattern ensures a class has only one instance and provides a global point of access to it.
// Key features:
// 1. Private constructor prevents external instantiation.
// 2. Static method (getInstance) provides access to the single instance, creating it on first call (lazy initialization).
// 3. Static member variable holds the single instance.

class Singleton {
private: 
    static Singleton* instance; // Static member to hold the single instance
    Singleton() {
        cout << "Singleton instance created!" << endl;
    }
public: 
    static Singleton* getInstance() {
        if (!instance) {
            instance = new Singleton();
        }
        return instance;
    }

};

// Initialize the static member variable to nullptr
Singleton* Singleton::instance = nullptr;

int main() {
    // Attempt to create multiple instances of Singleton
    Singleton* s1 = Singleton::getInstance(); // Creates the instance
    Singleton* s2 = Singleton::getInstance(); // Returns the existing instance
    Singleton* s3 = Singleton::getInstance(); // Returns the existing instance

    // Verify that all pointers point to the same instance
    if (s1 == s2 && s2 == s3) {
        cout << "All instances are the same!" << endl;
    } else {
        cout << "Instances are different!" << endl;
    }

    return 0;
}