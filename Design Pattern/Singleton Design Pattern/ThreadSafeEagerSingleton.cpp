#include<iostream>
using namespace std;

// Thread-Safe Eager Singleton  
// Is this Meyer’s Singleton Implementation? 
// No, this is not Meyer's Singleton implementation. 
// Meyer's Singleton is a specific implementation of the Singleton design pattern that uses a static local variable to ensure thread safety and lazy initialization. 
// In contrast, the implementation shown here is an eager initialization approach where the singleton instance is created at the time of class loading, ensuring thread safety without the need for synchronization.
// This implementation creates the singleton instance at the time of class loading, ensuring thread safety without the need for synchronization
class Singleton {
    private: 
        static Singleton* instance; // Static member to hold the single instance
        Singleton() {
            cout << "Singleton instance created!" << endl;
        }
    public:
    static Singleton* getInstance() {
        return instance; // Return the already created instance
    }
};
// Explain this syntax - The static member variable 'instance' is initialized with a new instance of the Singleton class at the time of class loading. 
// This means that the singleton instance is created before any thread accesses it, ensuring thread safety without the need for locks or synchronization mechanisms.
Singleton* Singleton::instance = new Singleton(); // Eagerly create the singleton instance at class loading

int main() {
    Singleton* s1 = Singleton::getInstance(); // Returns the existing instance
    Singleton* s2 = Singleton::getInstance(); // Returns the existing instance

    if(s1 == s2) {
        cout << "Both instances are the same!" << endl;
    } else {
        cout << "Instances are different!" << endl;
    }
    return 0;
}