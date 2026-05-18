#include<iostream>
#include<string>
using namespace std;

// Encapsulation is the process of bundling data and methods that operate on that data within a single unit, such as a class. 
// It restricts direct access to some of an object's components, which can prevent the accidental modification of data. 
// In C++, encapsulation is achieved using access specifiers: public, private, and protected.

class BankAccount {
private:
    string accountHolderName;
    double balance; 
public:
    BankAccount(string name, double initialBalance) {
        accountHolderName = name;
        balance = initialBalance;       
    }
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "Deposited: " << amount << ". New Balance: " << balance << endl;
        } else {
            cout << "Invalid deposit amount" << endl;
        }
    }
    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            cout << "Withdrew: " << amount << ". New Balance: " << balance << endl;
        } else {
            cout << "Invalid withdrawal amount or insufficient funds" << endl;
        }
    }
    //getter and setter for accountHolderName
    string getAccountHolderName() {
        return accountHolderName;
    }
    void setAccountHolderName(string name) {
        accountHolderName = name;
    }
    // getter for balance (no setter for balance to prevent direct modification)
    double getBalance() {
        return balance;
    }
};

int main() {
    BankAccount account("John Doe", 1000.0);
    cout << "Account Holder: " << account.getAccountHolderName() << endl;
    cout << "Initial Balance: " << account.getBalance() << endl;
    account.deposit(500.0);
    account.withdraw(200.0);
    cout << "Final Balance: " << account.getBalance() << endl;
    // account.balance = 500.0; // This will cause a compilation error because balance is private
    return 0;
}