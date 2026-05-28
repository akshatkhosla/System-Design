#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <map>
#include <mutex>
#include <chrono>

using namespace std;

enum class VehicleType { CAR, MOTORCYCLE, TRUCK };
enum class SpotType { SMALL, MEDIUM, LARGE };

// ─────────────────────────────────────────────────────────────────────────────
// SECTION 1 — VEHICLE HIERARCHY
// FR: supports Motorcycle, Car, Truck
// ─────────────────────────────────────────────────────────────────────────────

class Vehicle {
protected:
    VehicleType type_;
    string licensePlate_;
public:
    Vehicle(VehicleType type, const string& licensePlate) : type_(type), licensePlate_(licensePlate) {}
    const string &getLicensePlate() const { return licensePlate_; }
    VehicleType getType() const { return type_; }
    virtual string typeName() const = 0;
    virtual ~Vehicle() = default;
};

class Car : public Vehicle {
public:
    Car(const string& licensePlate) : Vehicle(VehicleType::CAR, licensePlate) {}
    string typeName() const override { return "Car"; }
};

class Motorcycle : public Vehicle {
public:
    Motorcycle(const string& licensePlate) : Vehicle(VehicleType::MOTORCYCLE, licensePlate) {}
    string typeName() const override { return "Motorcycle"; }
};

class Truck : public Vehicle {
public:
    // VehicleType::TRUCK is passed to the base Vehicle constructor to set the type_ member.
    Truck(const string& licensePlate) : Vehicle(VehicleType::TRUCK, licensePlate) {}
    string typeName() const override { return "Truck"; }
};

// ─────────────────────────────────────────────────────────────────────────────
// SECTION 2 — PARKING SPOT HIERARCHY
// NFR: extensible — add new spot type by subclassing only
// ─────────────────────────────────────────────────────────────────────────────

class ParkingSpot {
protected:
    int spotId_;
    int floorNumber_;
    bool isOccupied_ = false;
    SpotType spotType_;
    Vehicle* parkedVehicle_ = nullptr; // Pointer to the vehicle currently parked in this spot (nullptr if empty)
public:
    ParkingSpot(int spotId, int floorNumber, SpotType spotType) : spotId_(spotId), floorNumber_(floorNumber), spotType_(spotType) {}
    virtual bool canFitVehicle(VehicleType type) const = 0;
    virtual string typeName() const = 0;
    virtual ~ParkingSpot() = default;

    bool parkVehicle(Vehicle* vehicle) {
        if (isOccupied_) {
            cout << "Spot " << spotId_ << " is already occupied.\n";
            return false;
        }
        if (!canFitVehicle(vehicle->getType())) {
            cout << vehicle->typeName() << " cannot fit in " << typeName() << " spot.\n";
            return false;
        }
        parkedVehicle_ = vehicle;
        isOccupied_ = true;
        cout << "Parked " << vehicle->typeName() << " with license plate " << vehicle->getLicensePlate() << " in spot " << spotId_ << ".\n";
        return true;
    }
    void vacateSpot() {
        if (!isOccupied_) {
            cout << "Spot " << spotId_ << " is already vacant.\n";
            return;
        }
        cout << "Vacating spot " << spotId_ << " from vehicle with license plate " << parkedVehicle_->getLicensePlate() << ".\n";
        parkedVehicle_ = nullptr;
        isOccupied_ = false;
    }
    int getSpotId() const { return spotId_; }
    int getFloorNumber() const { return floorNumber_; }
    SpotType getSpotType() const { return spotType_; }
    bool isOccupied() const { return isOccupied_; }
};

class SmallSpot : public ParkingSpot {
public:
    SmallSpot(int spotId, int floorNumber) : ParkingSpot(spotId, floorNumber, SpotType::SMALL) {}
    bool canFitVehicle(VehicleType type) const override {
        return type == VehicleType::MOTORCYCLE;
    }
    string typeName() const override { return "Small"; }
};

class MediumSpot : public ParkingSpot {
public:
    MediumSpot(int spotId, int floorNumber) : ParkingSpot(spotId, floorNumber, SpotType::MEDIUM) {}
    bool canFitVehicle(VehicleType type) const override {
        return type == VehicleType::MOTORCYCLE || type == VehicleType::CAR;
    }
    string typeName() const override { return "Medium"; }
};

class LargeSpot : public ParkingSpot {
public:
    LargeSpot(int spotId, int floorNumber) : ParkingSpot(spotId, floorNumber, SpotType::LARGE) {}
    bool canFitVehicle(VehicleType /*type*/) const override {
        return true;
    }
    string typeName() const override { return "Large"; }
};

// ─────────────────────────────────────────────────────────────────────────────
// SECTION 3 — PAYMENT STRATEGY (Strategy pattern)
// NFR: add new payment method without rewrite — implement this interface
// ─────────────────────────────────────────────────────────────────────────────

class IPaymentStrategy {
public:
    virtual bool process(double amount) = 0; // Returns true if payment is successful
    virtual string methodName() const = 0;
    virtual ~IPaymentStrategy() = default;
};

class CreditCardPayment : public IPaymentStrategy {
public:
    bool process(double amount) override {
        cout << "Processing credit card payment of $" << amount << "...\n";
        // Simulate payment processing logic here (e.g., validate card, charge amount)
        cout << "Credit card payment successful.\n";
        return true; // Assume payment is always successful for this example
    }
    string methodName() const override { return "Credit Card"; }
};

class CashPayment : public IPaymentStrategy {
public:
    bool process(double amount) override {
        cout << "Processing cash payment of $" << amount << "...\n";
        // Simulate cash payment logic here (e.g., accept cash, provide change)
        cout << "Cash payment successful.\n";
        return true; // Assume payment is always successful for this example
    }
    string methodName() const override { return "Cash"; }
};

// Future: UpiPayment, WalletPayment — add here, zero changes elsewhere

// ─────────────────────────────────────────────────────────────────────────────
// SECTION 4 — PRICING STRATEGY (Strategy pattern)
// NFR: add new pricing model without rewrite — implement this interface
// FR:  flat rate based on timings + different rate per vehicle type
// ─────────────────────────────────────────────────────────────────────────────

class IPricingStrategy {
public:
    virtual double calculateFees(double hoursParked, const Vehicle* vehicle) = 0;
    virtual string modelName() const = 0;
    virtual ~IPricingStrategy() = default;
};

class FlatRatePricing : public IPricingStrategy {
public:
    double calculateFees(double hoursParked, const Vehicle* vehicle) override {
        double baseRate = 20.0; // Base rate per hour
        double typeMultiplier = 1.0; // Default multiplier for cars
        if (vehicle->getType() == VehicleType::MOTORCYCLE) {
            typeMultiplier = 0.5; // Motorcycles pay half the rate
        } else if (vehicle->getType() == VehicleType::TRUCK) {
            typeMultiplier = 2.0; // Trucks pay double the rate
        }
        double totalFee = baseRate * hoursParked * typeMultiplier;
        cout << "Calculated parking fee: $" << totalFee << " for " << hoursParked << " hours and vehicle type " << vehicle->typeName() << ".\n";
        return totalFee;
    }
    string modelName() const override { return "Flat Rate"; }
};
 
class HourlyRatePricing : public IPricingStrategy {
public:
    double calculateFees(double hoursParked, const Vehicle* vehicle) override {
        double hourlyRate = 5.0; // Base hourly rate
        double typeMultiplier = 1.0; // Default multiplier for cars
        if (vehicle->getType() == VehicleType::MOTORCYCLE) {
            typeMultiplier = 0.5; // Motorcycles pay half the rate
        } else if (vehicle->getType() == VehicleType::TRUCK) {
            typeMultiplier = 2.0; // Trucks pay double the rate
        }
        double totalFee = hourlyRate * hoursParked * typeMultiplier;
        cout << "Calculated parking fee: $" << totalFee << " for " << hoursParked << " hours and vehicle type " << vehicle->typeName() << ".\n";
        return totalFee;
    }
    string modelName() const override { return "Hourly Rate"; }
};

// ─────────────────────────────────────────────────────────────────────────────
// SECTION 5 — TICKET
// FR: vehicle enters → gets a ticket; vehicle exits → pays based on duration
// ─────────────────────────────────────────────────────────────────────────────

class PaymentRecord {
public:
    double amountPaid_;
    string paymentMethod_;
    chrono::system_clock::time_point paymentTime_;
};

class Ticket {
private:
    string ticketId_;
    Vehicle* vehicle_;
    chrono::system_clock::time_point entryTime_;
    chrono::system_clock::time_point exitTime_;
    bool isPaid_ = false;
    ParkingSpot* assignedSpot_;
    optional<PaymentRecord> paymentRecord_; // Stores payment details if paid
public:
    Ticket(const string& ticketId, Vehicle* vehicle, ParkingSpot* spot)
        : ticketId_(ticketId), vehicle_(vehicle), entryTime_(chrono::system_clock::now()), assignedSpot_(spot) {}
    double durationHours() const {
        auto duration = chrono::duration_cast<chrono::minutes>(exitTime_ - entryTime_).count();
        return duration / 60.0; // Convert minutes to hours
    }
    void recordExit() {
        exitTime_ = chrono::system_clock::now();
    }
    void recordPayment(double amount, const string& method) {
        paymentRecord_ = PaymentRecord{amount, method, chrono::system_clock::now()};
        isPaid_ = true;
    }
    Vehicle*      getVehicle()      const { return vehicle_; }
    ParkingSpot*  getAssignedSpot() const { return assignedSpot_; }
    string        getTicketId()     const { return ticketId_; }
    void printTicketDetails() const {
        cout << "Ticket ID: " << ticketId_ << "\n";
        cout << "Vehicle: " << vehicle_->typeName() << " with license plate " << vehicle_->getLicensePlate() << "\n";
        cout << "Entry Time: " << chrono::system_clock::to_time_t(entryTime_) << "\n";
        if (isPaid_) {
            cout << "Payment Amount: $" << paymentRecord_->amountPaid_ << "\n";
            cout << "Payment Method: " << paymentRecord_->paymentMethod_ << "\n";
            cout << "Payment Time: " << chrono::system_clock::to_time_t(paymentRecord_->paymentTime_) << "\n";
        } else {
            cout << "Payment Status: Unpaid\n";
        }
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// SECTION 6 — SPOT ASSIGNMENT STRATEGY (Strategy pattern)
// NFR: extensible — add new assignment algorithm by implementing interface
// ─────────────────────────────────────────────────────────────────────────────

class ParkingFloor; // Forward declaration to avoid circular dependency

class ISpotAssignmentStrategy {
public:
    virtual ParkingSpot* assignSpot(ParkingFloor& floor, Vehicle* vehicle) = 0;
    virtual ~ISpotAssignmentStrategy() = default;
};

// ─────────────────────────────────────────────────────────────────────────────
// SECTION 7 — PARKING FLOOR
// NFR: thread-safe — mutex protects the find + mark-occupied sequence
// ─────────────────────────────────────────────────────────────────────────────

class ParkingFloor {
private:
    int floorNumber_;
    vector<ParkingSpot*> spots_;
    mutex floorMutex_; // Mutex to protect concurrent access to parking spots
public:
    ParkingFloor(int floorNumber) : floorNumber_(floorNumber) {}
    void addSpot(ParkingSpot* spot) {
        spots_.push_back(spot);
    }
    vector<ParkingSpot*>& getSpots() { return spots_; }
    int getFloorNumber() const { return floorNumber_; }
    // Finds an available spot for the vehicle using the provided assignment strategy.
    ParkingSpot* findAndAssignSpot(Vehicle* vehicle, ISpotAssignmentStrategy& strategy) {
        lock_guard<mutex> lock(floorMutex_); // Lock the mutex to ensure thread safety during spot assignment
        return strategy.assignSpot(*this, vehicle);
    }
    int availableSpotsCount() const {
        int count = 0;
        for (const auto& spot : spots_) {
            if (!spot->isOccupied()) {
                count++;
            }
        }
        return count;
    }
    void printAvailableSpots() const {
        cout << "Available spots on floor " << floorNumber_ << ": ";
        for (const auto& spot : spots_) {
            if (!spot->isOccupied()) {
                cout << spot->getSpotId() << " (" << spot->typeName() << ") ";
            }
        }
        cout << "\n";
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// SECTION 8 — SPOT ASSIGNMENT STRATEGY IMPLEMENTATION
// (defined after ParkingFloor is complete)
// ─────────────────────────────────────────────────────────────────────────────

class FirstAvailableStrategy : public ISpotAssignmentStrategy {
public:
    // Called with floorMutex_ already held — must not re-acquire it
    ParkingSpot* assignSpot(ParkingFloor& floor, Vehicle* vehicle) override {
        for (ParkingSpot* spot : floor.getSpots()) {
            if (!spot->isOccupied() && spot->canFitVehicle(vehicle->getType())) {
                spot->parkVehicle(vehicle);
                return spot;
            }
        }
        return nullptr;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// SECTION 9 — PARKING LOT (Facade pattern)
// Single entry point: orchestrates floors, strategies, ticket lifecycle
// ─────────────────────────────────────────────────────────────────────────────

class ParkingLot {
private:
    vector<ParkingFloor*>    floors_;
    map<string, Ticket*>     activeTickets_;
    ISpotAssignmentStrategy* assignStrategy_;
    IPricingStrategy*        pricingStrategy_;
    mutex                    ticketMutex_;   // guards activeTickets_ map
    int                      ticketCounter_ = 0;

    string generateTicketId() {
        return "TKT-" + to_string(++ticketCounter_);
    }

public:
    ParkingLot(ISpotAssignmentStrategy* assignStrategy, IPricingStrategy* pricingStrategy)
        : assignStrategy_(assignStrategy), pricingStrategy_(pricingStrategy) {}

    void addFloor(ParkingFloor* floor) {
        floors_.push_back(floor);
    }

    void setPricingStrategy(IPricingStrategy* strategy) {
        pricingStrategy_ = strategy;
    }

    // FR: vehicle enters → spot assigned, ticket issued
    Ticket* enter(Vehicle* vehicle) {
        for (ParkingFloor* floor : floors_) {
            ParkingSpot* spot = floor->findAndAssignSpot(vehicle, *assignStrategy_);
            if (spot) {
                string id = generateTicketId();
                Ticket* ticket = new Ticket(id, vehicle, spot);
                {
                    lock_guard<mutex> lock(ticketMutex_);
                    activeTickets_[id] = ticket;
                }
                cout << "[ENTRY] " << vehicle->typeName() << " (" << vehicle->getLicensePlate()
                     << ") → ticket " << id << ", floor " << floor->getFloorNumber()
                     << ", spot " << spot->getSpotId() << " (" << spot->typeName() << ")\n";
                return ticket;
            }
        }
        cout << "[ENTRY] No available spot for " << vehicle->typeName()
             << " (" << vehicle->getLicensePlate() << ")\n";
        return nullptr;
    }

    // FR: vehicle exits → fee calculated, payment processed, spot freed
    double exit(const string& ticketId, IPaymentStrategy* paymentStrategy) {
        Ticket* ticket = nullptr;
        {
            lock_guard<mutex> lock(ticketMutex_);
            auto it = activeTickets_.find(ticketId);
            if (it == activeTickets_.end()) {
                cout << "[EXIT] Ticket " << ticketId << " not found.\n";
                return 0.0;
            }
            ticket = it->second;
            activeTickets_.erase(it);
        }

        ticket->recordExit();
        double hours = ticket->durationHours();
        double fee   = pricingStrategy_->calculateFees(hours, ticket->getVehicle());

        if (!paymentStrategy->process(fee)) {
            cout << "[EXIT] Payment failed — re-inserting ticket.\n";
            lock_guard<mutex> lock(ticketMutex_);
            activeTickets_[ticketId] = ticket;
            return 0.0;
        }

        ticket->recordPayment(fee, paymentStrategy->methodName());
        ticket->getAssignedSpot()->vacateSpot();
        ticket->printTicketDetails();
        delete ticket;
        return fee;
    }

    // FR: query available spots per floor per vehicle type
    int availableSpots(int floorNumber, VehicleType type) const {
        for (ParkingFloor* floor : floors_) {
            if (floor->getFloorNumber() == floorNumber) {
                int count = 0;
                for (ParkingSpot* spot : floor->getSpots()) {
                    if (!spot->isOccupied() && spot->canFitVehicle(type))
                        ++count;
                }
                return count;
            }
        }
        return 0;
    }

    void printStatus() const {
        cout << "\n── Parking Lot Status ──\n";
        for (ParkingFloor* floor : floors_)
            floor->printAvailableSpots();
        cout << "────────────────────────\n\n";
    }

    ~ParkingLot() {
        for (auto& kv : activeTickets_) delete kv.second;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// MAIN — demo: enter, query, exit with payment
// ─────────────────────────────────────────────────────────────────────────────

int main() {
    // Build floors with spots
    ParkingFloor* floor1 = new ParkingFloor(1);
    floor1->addSpot(new SmallSpot(101, 1));
    floor1->addSpot(new SmallSpot(102, 1));
    floor1->addSpot(new MediumSpot(103, 1));
    floor1->addSpot(new MediumSpot(104, 1));
    floor1->addSpot(new LargeSpot(105, 1));

    ParkingFloor* floor2 = new ParkingFloor(2);
    floor2->addSpot(new MediumSpot(201, 2));
    floor2->addSpot(new LargeSpot(202, 2));
    floor2->addSpot(new LargeSpot(203, 2));

    // Strategies (stack-allocated — no heap needed for singletons)
    FirstAvailableStrategy assignStrategy;
    HourlyRatePricing      pricingStrategy;
    CreditCardPayment      cardPayment;
    CashPayment            cashPayment;

    // Build lot
    ParkingLot lot(&assignStrategy, &pricingStrategy);
    lot.addFloor(floor1);
    lot.addFloor(floor2);

    // Vehicles
    Car        car1("KA-01-1234");
    Motorcycle moto1("DL-05-5678");
    Truck      truck1("MH-12-9999");

    lot.printStatus();

    // Enter
    Ticket* t1 = lot.enter(&car1);
    Ticket* t2 = lot.enter(&moto1);
    Ticket* t3 = lot.enter(&truck1);

    lot.printStatus();

    // Query
    cout << "Cars available on floor 1: "
         << lot.availableSpots(1, VehicleType::CAR) << "\n\n";

    // Exit — save IDs before delete inside exit()
    string id1 = t1 ? t1->getTicketId() : "";
    string id2 = t2 ? t2->getTicketId() : "";
    string id3 = t3 ? t3->getTicketId() : "";

    if (!id1.empty()) lot.exit(id1, &cardPayment);
    if (!id2.empty()) lot.exit(id2, &cashPayment);
    if (!id3.empty()) lot.exit(id3, &cardPayment);

    lot.printStatus();

    // Cleanup: floors own their spots
    for (ParkingSpot* s : floor1->getSpots()) delete s;
    for (ParkingSpot* s : floor2->getSpots()) delete s;
    delete floor1;
    delete floor2;

    return 0;
} 