#include<iostream>
#include<string>
using namespace std;

// Inheritance is a fundamental object-oriented programming concept that allows a new class (called a derived class or child class) 
// to inherit properties and behaviors (data members and member functions) from an existing class (called a base class or parent class). 
// In C++, inheritance is implemented using the `:` symbol followed by the access specifier (public, protected, or private) and the name of the base class. 
// The derived class can add new members or override existing members of the base class to provide specific functionality.
class Car {
protected:
    string brandName;
    string model;
    int currentSpeed;
    bool isEngineOn;
public:
    Car(string brand, string model, int currentSpeed, bool isEngineOn) {
        this->brandName = brand;
        this->model = model;
        this->currentSpeed = currentSpeed;
        this->isEngineOn = isEngineOn;
    }
    void displayInfo() {
        cout << "Brand: " << brandName << ", Model: " << model << ", Current Speed: " << currentSpeed << ", Engine On: " << isEngineOn << endl;
    }
    void startEngine() {
        isEngineOn = true;
        cout << brandName << " " << model << " " << "Engine started" << endl;
    }
    void accelarate(int speed) {
        cout << brandName << " " << model << " " << "Accelerating to speed: " << speed << endl;
    }
    void applyBreak() {
        currentSpeed -= 20;
        if(currentSpeed < 0) currentSpeed = 0;
        cout << brandName << " " << model << " " << "Applying brakes" << endl;
    }
    void stopEngine() {
        isEngineOn = false;
        cout << brandName << " " << model << " " << "Engine stopped" << endl;
    }
};

// Derived class for manual transmission cars
class ManualCar : public Car {
private:
    int currentGear;    
public:
    ManualCar(string brand, string model, int currentSpeed, bool isEngineOn) : Car(brand, model, currentSpeed, isEngineOn   ) {}
    void shiftGear(int gear) {
        cout << brandName << " " << model << " " << "Shifting to gear: " << gear << endl;
    }

};  

class ElectricCar : public Car {
private:
    int batteryLevel;
public:
    ElectricCar(string brand, string model, int currentSpeed, bool isEngineOn) : Car(brand, model, currentSpeed, isEngineOn) {}
    void chargeBattery() {
        cout << brandName << " " << model << " " << "Charging battery" << endl;
    }
};

int main() {
    ManualCar myManualCar("Toyota", "Corolla", 0, false);
    myManualCar.displayInfo();
    myManualCar.startEngine();
    myManualCar.shiftGear(1);
    myManualCar.accelarate(60);
    myManualCar.applyBreak();
    myManualCar.stopEngine();

    cout  << "-----------------------------" << endl;

    ElectricCar myElectricCar("Tesla", "Model S", 0, false);
    myElectricCar.displayInfo();
    myElectricCar.startEngine();
    myElectricCar.accelarate(100);
    myElectricCar.chargeBattery();
    myElectricCar.stopEngine();

    return 0;
}