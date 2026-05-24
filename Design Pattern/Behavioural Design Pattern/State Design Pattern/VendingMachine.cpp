#include<iostream>
#include<string>
#include<map>

using namespace std;

// State Design Pattern allows an object to alter its behavior when its internal state changes.
// The object will appear to change its class.
// This pattern is also known as State Machine.

// Forward declaration of the context class to avoid circular dependency.
class VendingMachine;

// State interface — every concrete state implements these transitions.
class IVendingMachineState {
public:
    virtual void selectItem(VendingMachine* machine, const string& item) = 0;
    virtual void insertCoin(VendingMachine* machine, double amount) = 0;
    virtual void dispense(VendingMachine* machine) = 0;
    virtual void cancel(VendingMachine* machine) = 0;
    virtual ~IVendingMachineState() = default;
};

// Context — holds current state and item/balance data.
class VendingMachine {
private:
    IVendingMachineState* currentState;
    string selectedItem;
    double balance;
    map<string, double> inventory; // item -> price

public:
    VendingMachine() : currentState(nullptr), balance(0.0) {}

    void setState(IVendingMachineState* state) {
        currentState = state;
    }

    void addItem(const string& item, double price) {
        inventory[item] = price;
    }

    double getItemPrice(const string& item) {
        if (inventory.count(item)) return inventory[item];
        return -1;
    }

    void setSelectedItem(const string& item) { selectedItem = item; }
    string getSelectedItem() const { return selectedItem; }

    void addBalance(double amount) { balance += amount; }
    double getBalance() const { return balance; }
    void resetBalance() { balance = 0.0; }

    // Delegates all actions to the current state.
    void selectItem(const string& item) { currentState->selectItem(this, item); }
    void insertCoin(double amount)      { currentState->insertCoin(this, amount); }
    void dispense()                     { currentState->dispense(this); }
    void cancel()                       { currentState->cancel(this); }
};

// ── Concrete States ──────────────────────────────────────────────────────────

class IdleState;
class ItemSelectedState;
class PaymentPendingState;
class DispensingState;

// Idle — machine is waiting for user input.
class IdleState : public IVendingMachineState {
public:
    // User selects an item, transition to ItemSelected state if valid.
    void selectItem(VendingMachine* machine, const string& item) override;
    void insertCoin(VendingMachine* machine, double amount) override {
        cout << "[Idle] Please select an item first.\n";
    }
    void dispense(VendingMachine* machine) override {
        cout << "[Idle] No item selected or paid for.\n";
    }
    void cancel(VendingMachine* machine) override {
        cout << "[Idle] Nothing to cancel.\n";
    }
};

// ItemSelected — user picked an item, waiting for payment.
class ItemSelectedState : public IVendingMachineState {
public:
    void selectItem(VendingMachine* machine, const string& item) override {
        cout << "[ItemSelected] Item already selected. Please insert coins or cancel.\n";
    }
    void insertCoin(VendingMachine* machine, double amount) override;
    void dispense(VendingMachine* machine) override {
        cout << "[ItemSelected] Please insert coins first.\n";
    }
    void cancel(VendingMachine* machine) override;
};

// PaymentPending — coins inserted, checking if balance is sufficient.
class PaymentPendingState : public IVendingMachineState {
public:
    void selectItem(VendingMachine* machine, const string& item) override {
        cout << "[Payment] Payment in progress. Cannot select another item.\n";
    }
    void insertCoin(VendingMachine* machine, double amount) override;
    void dispense(VendingMachine* machine) override;
    void cancel(VendingMachine* machine) override;
};

// Dispensing — item is being dispensed, then returns to Idle.
class DispensingState : public IVendingMachineState {
public:
    void selectItem(VendingMachine* machine, const string& item) override {
        cout << "[Dispensing] Please wait, dispensing in progress.\n";
    }
    void insertCoin(VendingMachine* machine, double amount) override {
        cout << "[Dispensing] Please wait, dispensing in progress.\n";
    }
    void dispense(VendingMachine* machine) override;
    void cancel(VendingMachine* machine) override {
        cout << "[Dispensing] Cannot cancel, already dispensing.\n";
    }
};

// ── State singletons (one instance per state is sufficient) ──────────────────

IdleState*          idleState          = new IdleState();
ItemSelectedState*  itemSelectedState  = new ItemSelectedState();
PaymentPendingState* paymentPendingState = new PaymentPendingState();
DispensingState*    dispensingState    = new DispensingState();

// ── Method bodies defined here because they reference state singletons ───────
// These methods call machine->setState(someOtherState), so they need the
// singleton variables (idleState, itemSelectedState, etc.) to already exist.
// Those singletons are declared just above this block, after all class definitions.
// Defining these bodies inside the class would fail because the singletons
// aren't declared yet at that point in the file.

// Defined outside: calls machine->setState(itemSelectedState) — singleton declared above.
void IdleState::selectItem(VendingMachine* machine, const string& item) {
    double price = machine->getItemPrice(item);
    if (price < 0) {
        cout << "[Idle] Item '" << item << "' not available.\n";
        return;
    }
    machine->setSelectedItem(item);
    cout << "[Idle] Item '" << item << "' selected. Price: $" << price << ". Please insert coins.\n";
    machine->setState(itemSelectedState);
}

// Defined outside: calls machine->setState(paymentPendingState) — singleton declared above.
void ItemSelectedState::insertCoin(VendingMachine* machine, double amount) {
    machine->addBalance(amount);
    double price = machine->getItemPrice(machine->getSelectedItem());
    cout << "[ItemSelected] Inserted $" << amount << ". Balance: $" << machine->getBalance() << ".\n";
    machine->setState(paymentPendingState);
}

// Defined outside: calls machine->setState(idleState) — singleton declared above.
void ItemSelectedState::cancel(VendingMachine* machine) {
    cout << "[ItemSelected] Cancelled. Returning to idle.\n";
    machine->setSelectedItem("");
    machine->setState(idleState);
}

// Defined outside: calls machine->setState(dispensingState) inside dispense() — kept here for consistency.
void PaymentPendingState::insertCoin(VendingMachine* machine, double amount) {
    machine->addBalance(amount);
    cout << "[Payment] Inserted $" << amount << ". Balance: $" << machine->getBalance() << ".\n";
}

// Defined outside: calls machine->setState(dispensingState) — singleton declared above.
void PaymentPendingState::dispense(VendingMachine* machine) {
    double price   = machine->getItemPrice(machine->getSelectedItem());
    double balance = machine->getBalance();
    if (balance < price) {
        cout << "[Payment] Insufficient balance. Need $" << (price - balance) << " more.\n";
        return;
    }
    cout << "[Payment] Payment accepted. Dispensing...\n";
    machine->setState(dispensingState);
    machine->dispense();
}

// Defined outside: calls machine->setState(idleState) — singleton declared above.
void PaymentPendingState::cancel(VendingMachine* machine) {
    cout << "[Payment] Cancelled. Refunding $" << machine->getBalance() << ".\n";
    machine->resetBalance();
    machine->setSelectedItem("");
    machine->setState(idleState);
}

// Defined outside: calls machine->setState(idleState) — singleton declared above.
void DispensingState::dispense(VendingMachine* machine) {
    double change = machine->getBalance() - machine->getItemPrice(machine->getSelectedItem());
    cout << "[Dispensing] Here is your '" << machine->getSelectedItem() << "'!\n";
    if (change > 0)
        cout << "[Dispensing] Returning change: $" << change << ".\n";
    machine->resetBalance();
    machine->setSelectedItem("");
    machine->setState(idleState);
}

// ── main ─────────────────────────────────────────────────────────────────────

int main() {
    VendingMachine machine;
    machine.setState(idleState);

    machine.addItem("Coke",   1.50);
    machine.addItem("Chips",  1.00);
    machine.addItem("Water",  0.75);

    cout << "\n--- Select and pay exact amount ---\n";
    machine.selectItem("Coke");
    machine.insertCoin(1.50);
    machine.dispense();

    cout << "\n--- Insert coins in multiple steps ---\n";
    machine.selectItem("Chips");
    machine.insertCoin(0.50);
    machine.insertCoin(0.50);
    machine.dispense();

    cout << "\n--- Cancel mid-payment ---\n";
    machine.selectItem("Water");
    machine.insertCoin(0.50);
    machine.cancel();

    cout << "\n--- Invalid item ---\n";
    machine.selectItem("Coffee");

    cout << "\n--- Overpay (change returned) ---\n";
    machine.selectItem("Water");
    machine.insertCoin(1.00);
    machine.dispense();

    delete idleState;
    delete itemSelectedState;
    delete paymentPendingState;
    delete dispensingState;

    return 0;
}
