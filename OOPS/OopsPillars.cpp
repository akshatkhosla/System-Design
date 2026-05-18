#include<iostream>
#include<string>
using namespace std;

// ============================================================
// ABSTRACTION: Abstract base class hides implementation details
// and exposes only the interface that all cars must follow.
// ============================================================
class Car {
protected:
    // ENCAPSULATION: data is private/protected — only accessible
    // through controlled methods, not directly from outside.
    string brandName;
    string model;
    int currentSpeed;
    bool isEngineOn;

public:
    Car(string brand, string model) {
        this->brandName = brand;
        this->model = model;
        this->currentSpeed = 0;
        this->isEngineOn = false;
    }

    // Getters — controlled read access to encapsulated data
    string getBrand() { return brandName; }
    string getModel() { return model; }
    int getCurrentSpeed() { return currentSpeed; }
    bool getEngineStatus() { return isEngineOn; }

    void displayInfo() {
        cout << "[" << brandName << " " << model << "] "
             << "Speed: " << currentSpeed << " km/h | "
             << "Engine: " << (isEngineOn ? "ON" : "OFF") << endl;
    }

    // Non-virtual methods with shared logic
    void startEngine() {
        isEngineOn = true;
        cout << brandName << " " << model << ": Engine started" << endl;
    }

    void stopEngine() {
        isEngineOn = false;
        currentSpeed = 0;
        cout << brandName << " " << model << ": Engine stopped" << endl;
    }

    void applyBrake() {
        currentSpeed -= 20;
        if (currentSpeed < 0) currentSpeed = 0;
        cout << brandName << " " << model << ": Brakes applied. Speed: " << currentSpeed << " km/h" << endl;
    }

    // ABSTRACTION: Pure virtual function — derived classes MUST implement this.
    // Each car type accelerates differently, so we leave it to them.
    virtual void accelerate(int amount) = 0;

    // STATIC POLYMORPHISM (function overloading): accelerate with a default
    // amount vs. a specific amount — resolved at compile time.
    void accelerate() {
        accelerate(20); // delegates to the overridden version
    }

    virtual ~Car() {}
};


// ============================================================
// INHERITANCE + ENCAPSULATION + DYNAMIC POLYMORPHISM
// ManualCar inherits from Car and adds gear-specific behavior.
// ============================================================
class ManualCar : public Car {
private:
    int currentGear; // encapsulated — only ManualCar manages this

public:
    ManualCar(string brand, string model) : Car(brand, model) {
        this->currentGear = 0;
    }

    int getCurrentGear() { return currentGear; }

    void shiftGear(int gear) {
        if (!isEngineOn) {
            cout << brandName << " " << model << ": Turn on the engine first!" << endl;
            return;
        }
        currentGear = gear;
        cout << brandName << " " << model << ": Shifted to gear " << currentGear << endl;
    }

    // DYNAMIC POLYMORPHISM: overrides base class pure virtual.
    // Manual cars accelerate based on gear — more gradual.
    void accelerate(int amount) override {
        if (!isEngineOn) {
            cout << brandName << " " << model << ": Can't accelerate, engine is off!" << endl;
            return;
        }
        currentSpeed += amount;
        cout << brandName << " " << model << ": Accelerating (manual). Speed: " << currentSpeed << " km/h" << endl;
    }
};


// ============================================================
// INHERITANCE + ENCAPSULATION + DYNAMIC POLYMORPHISM
// ElectricCar inherits from Car and adds battery-specific behavior.
// ============================================================
class ElectricCar : public Car {
private:
    int batteryLevel; // encapsulated — only ElectricCar manages this

public:
    ElectricCar(string brand, string model, int batteryLevel)
        : Car(brand, model), batteryLevel(batteryLevel) {}

    int getBatteryLevel() { return batteryLevel; }

    void chargeBattery(int amount) {
        batteryLevel += amount;
        if (batteryLevel > 100) batteryLevel = 100;
        cout << brandName << " " << model << ": Battery charged to " << batteryLevel << "%" << endl;
    }

    // DYNAMIC POLYMORPHISM: overrides base class pure virtual.
    // Electric cars accelerate faster and drain battery.
    void accelerate(int amount) override {
        if (!isEngineOn) {
            cout << brandName << " " << model << ": Can't accelerate, engine is off!" << endl;
            return;
        }
        if (batteryLevel <= 0) {
            cout << brandName << " " << model << ": Battery dead! Please charge." << endl;
            return;
        }
        currentSpeed += amount + 10; // electric motors give extra torque
        batteryLevel -= 5;
        cout << brandName << " " << model << ": Accelerating (electric). Speed: " << currentSpeed
             << " km/h | Battery: " << batteryLevel << "%" << endl;
    }
};


int main() {
    // POLYMORPHISM via base class pointers —
    // both objects are treated as Car*, but behave differently at runtime.
    Car* car1 = new ManualCar("Toyota", "Corolla");
    Car* car2 = new ElectricCar("Tesla", "Model S", 80);

    cout << "========== Manual Car ==========" << endl;
    car1->displayInfo();
    car1->startEngine();

    // Static polymorphism: accelerate() with no args calls accelerate(20) via overloading
    car1->accelerate();

    // We need the ManualCar-specific shiftGear, so downcast
    ManualCar* manualCar = dynamic_cast<ManualCar*>(car1);
    if (manualCar) {
        manualCar->shiftGear(2);
    }

    car1->accelerate(40); // dynamic polymorphism — ManualCar's version runs
    car1->applyBrake();
    car1->stopEngine();
    car1->displayInfo();

    cout << "\n========== Electric Car ==========" << endl;
    car2->displayInfo();
    car2->startEngine();
    car2->accelerate();      // static polymorphism — calls accelerate(20) -> ElectricCar's override
    car2->accelerate(50);    // dynamic polymorphism — ElectricCar's version runs
    car2->applyBrake();

    // Downcast to access ElectricCar-specific behavior
    ElectricCar* electricCar = dynamic_cast<ElectricCar*>(car2);
    if (electricCar) {
        electricCar->chargeBattery(20);
    }

    car2->stopEngine();
    car2->displayInfo();

    delete car1;
    delete car2;

    return 0;
}
