#include<iostream>
#include<string>
using namespace std;

// Polymorphism is a fundamental concept in object-oriented programming that allows objects of different classes to be treated as objects of a common base class. 
// It enables a single interface to represent different underlying forms (data types). 
// In C++, polymorphism is achieved through function overloading, operator overloading, and virtual functions. 
// The most common form of polymorphism in C++ is achieved through virtual functions, which allow for dynamic (runtime) polymorphism. 
// When a base class declares a function as virtual, it can be overridden in any derived class. 
// When a base class pointer or reference is used to call a virtual function, the version of the function that is executed is determined by the actual type of the object being pointed to, rather than the type of the pointer or reference.   

//Dynamic Polymorphism using virtual functions
// Method overriding is a feature that allows a derived class to provide a specific implementation of a function that is already defined in its base class.
class Car {
public:
    int speed;
    virtual void startEngine() {
        cout << "Starting the engine of the car" << endl;
    }
    virtual void accelerate(int speed) = 0; // Pure virtual function, making Car an abstract class
    virtual void applyBreak() {
        cout << "Applying brakes" << endl;
    }
    virtual void stopEngine() {
        cout << "Stopping the engine of the car" << endl;
    }   
    virtual ~Car() {}; // Virtual destructor to ensure proper cleanup of derived class objects through base class pointers
};

class ManualCar : public Car {
public:
    // DYNAMIC POLYMORPHISM: overrides base class pure virtual.
    void accelerate(int speed) override {
        speed += 20;
        cout << "Manual Car: Accelerating at speed " << speed << endl;
    } 
};

class ElectricCar : public Car {
public:
    // DYNAMIC POLYMORPHISM: overrides base class pure virtual.
    void accelerate(int speed) override {
        speed += 30;
        cout << "Electric Car: Accelerating at speed " << speed << endl;
    }
};

int main() {
    Car* car1 = new ManualCar();
    Car* car2 = new ElectricCar();

    car1->startEngine();
    car1->accelerate(50);
    car1->applyBreak();
    car1->stopEngine();

    cout << "----------------" << endl;

    car2->startEngine();
    car2->accelerate(50);
    car2->applyBreak();
    car2->stopEngine();

    delete car1; // Clean up memory
    delete car2; // Clean up memory

    return 0;
}