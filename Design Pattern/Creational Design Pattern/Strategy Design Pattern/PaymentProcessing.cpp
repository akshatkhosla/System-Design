#include<iostream>
#include<string>

using namespace std;

class IPaymentStrategy {
public:
    virtual void pay(double amount) = 0;
    virtual void getType() = 0;
    virtual ~IPaymentStrategy() = default;
};

class CreditCardPayment : public IPaymentStrategy {
private:
    string cardNumber;
    string cardHolderName;
    string expiryDate;
public:
    CreditCardPayment(const string& number, const string& holder, const string& expiry)       
     : cardNumber(number), cardHolderName(holder), expiryDate(expiry) {}
     void pay(double amount) override {
        cout << "Processing credit card payment of $" << amount << " for " << cardHolderName << endl;
    }
    void getType() override {
        cout << "Payment method: Credit Card" << endl;
    }
};

class PayPalPayment : public IPaymentStrategy {
private:
    string email;
    string password;
public:
    PayPalPayment(const string& email, const string& password) : email(email), password(password) {}
    void pay(double amount) override {
        cout << "Processing PayPal payment of $" << amount << " for " << email << endl;
    }
    void getType() override {
        cout << "Payment method: PayPal" << endl;
    }
};

class UPIPayment : public IPaymentStrategy {
private:
    string upiId;
public:
    UPIPayment(const string& upiId) : upiId(upiId) {}
    void pay(double amount) override {
        cout << "Processing UPI payment of $" << amount << " for " << upiId << endl;
    }
    void getType() override {
        cout << "Payment method: UPI" << endl;
    }
};

class PaymentProcessor {
private:
    IPaymentStrategy* strategy;
    double amount;
public:
    PaymentProcessor(IPaymentStrategy* strategy, double amount) : strategy(strategy), amount(amount) {}
    void processPayment() {
        strategy->getType();
        strategy->pay(amount);
    }
    void setStrategy(IPaymentStrategy* newStrategy) {
        strategy = newStrategy;
    }
};

int main() {
    CreditCardPayment creditCard("1234-5678-9012-3456", "John Doe", "12/25");
    PayPalPayment paypal("john.doe@example.com", "password123");
    UPIPayment upi("upi_id");

    PaymentProcessor processor(&creditCard, 100.0);
    processor.processPayment();

    processor.setStrategy(&paypal);
    processor.processPayment();

    processor.setStrategy(&upi);
    processor.processPayment();

    return 0;
}