#include<iostream>
#include<string>
using namespace std;

//Static Polymorphism, also known as compile-time polymorphism, is a type of polymorphism that is resolved during the compilation of the program. 
// It is achieved through function overloading and operator overloading. 
// In function overloading, multiple functions can have the same name but different parameters (either in number, type, or both). 
// The compiler determines which function to call based on the arguments passed to the function. 
// In operator overloading, operators can be redefined to work with user-defined types, allowing for more intuitive code when working with objects of those types.

class ManualCar {
private:
        string brandName;
        string model;
        int currentSpeed = 0; // Initializing currentSpeed to 0
        int currentGear = 0; // Initializing currentGear to 0
        bool isEngineOn;
public:
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
    // STATIC POLYMORPHISM (function overloading): accelerate with no args vs. a specific amount — resolved at compile time.
    void accelarate() {
        currentSpeed += 20;
        cout << brandName << " " << model << " " << "Current speed is " << currentSpeed << endl;
    }
    // Overloaded accelerate function that takes a specific speed increment
    void accelarate(int speed) {
        currentSpeed += speed;
        cout << brandName << " " << model << " " << "Current speed is " << currentSpeed << endl;
    }
    void applyBreak() {
        currentSpeed -= 20;
        if(currentSpeed < 0) currentSpeed = 0;
        cout << brandName << " " << model << " " << "Current speed is " << currentSpeed << endl;
    }
    void stopEngine() {
        isEngineOn = false;
        cout << brandName << " " << model << " " << "Engine stopped" << endl;
    }
};

int main() {
    ManualCar car;
    car.startEngine();
    car.accelarate();
    car.accelarate(30);
    car.applyBreak();
    car.stopEngine();
    return 0;
}