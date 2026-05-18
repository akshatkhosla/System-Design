#include<iostream>
#include<string>
using namespace std;

// Abstraction is the process of hiding the implementation details and showing only the functionality to the user. 
// In C++, abstraction is achieved using abstract classes and interfaces. An abstract class is a class
// that cannot be instantiated and is typically used as a base class. 
// It contains at least one pure virtual function, which is a function declared with the syntax `virtual returnType functionName(parameters) = 0;`.

// Abstract Class - Base class with pure virtual functions
class Car {
public:
    virtual void startEngine() = 0;
    virtual void shiftGear(int gear) = 0;
    virtual void accelarate() = 0;
    virtual void applyBreak() = 0;
    virtual void stopEngine() = 0;
};

// Concrete class (A class that provides implementation of abstract class)
class SportsCar : public Car {
public: 
    string brandName;
    string model;
    bool isEngineOn;
    int currentSpeed;
    int currentGear;

    SportsCar(string brand, string model) {
        this->brandName = brand;
        this->model = model;
        this->isEngineOn = false;
        this->currentSpeed = 0;
        this->currentGear = 0;
    }

    void startEngine() {
        isEngineOn = true;
        cout << brandName << " " << model << " " << "Engine started" << endl;
    }

    void shiftGear(int gear) {
        if (!isEngineOn) {
            cout << "Please turn on the engine" << endl;
            return;
        }
        currentGear = gear;
        cout << brandName << " " << model << " " << "Current gear is " << currentGear << endl;
    }

    void accelarate() {
        currentSpeed += 20;
        cout << brandName << " " << model << " " << "Current speed is " << currentSpeed << endl;
    }

    void applyBreak() {
        currentSpeed -= 20;
        if(currentSpeed < 0) currentSpeed = 0;
        cout << brandName << " " << model << " " << "Current speed is " << currentSpeed << endl;
    }

    void stopEngine() {
        isEngineOn = false;
        currentGear = 0;
        currentSpeed = 0;
        cout << brandName << " " << model << " " << "Engine turned OFF" << endl;
    }
};

int main() {
    Car *myCar = new SportsCar("Buggi", "2017");
    myCar->startEngine();
    myCar->shiftGear(1);
    myCar->accelarate();
    myCar->shiftGear(2);
    
    myCar->applyBreak();
    myCar->stopEngine();

    return 0;
}



