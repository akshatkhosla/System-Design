#include<iostream>
#include<vector>
#include<typeinfo>
#include<stdexcept>

using namespace std;

class DepositOnlyAccount {
    public:
    virtual void deposit (double amount) = 0;
};

class WithdrawableAccount : public DepositOnlyAccount {
    public:
    virtual void withdraw (double amount) = 0;
};

class SavingAccount : public WithdrawableAccount {
    private: 
    double balance;

    public:
    void deposit (double amount) {
        balance += amount;
        cout << "Deposited: " << amount << " New balance: " << balance << endl;
    }
    void withdraw (double amount) {
        if(balance < amount) {
            cout << "Insufficient funds in Savings account" << endl;
        }
        else {
            balance -= amount;
            cout << "Withrawn: " << amount << " from Savings Account. New Balance " << balance << endl;
        }
    }
};

class CurrentAccount : public WithdrawableAccount {
    private: 
    double balance;

    public:
    void deposit (double amount) {
        balance += amount;
        cout << "Deposited: " << amount << "New balance: " << balance << endl;
    }
    void withdraw (double amount) {
        if(balance < amount) {
            cout << "Insufficient funds in Current account" << endl;
        }
        else {
            balance -= amount;
            cout << "Withrawn: " << amount << " from Current Account. New Balance " << balance << endl;
        }
    }
};

class FDAccount : public DepositOnlyAccount {
    private:
    double balance;

    public:
    void deposit(double amount) {
        balance += amount;
        cout << "Deposited: " << amount << " New balance: " << balance << endl;
    }
};

class BankClient {
    private:
    vector<WithdrawableAccount*> withdrawableAccounts;
    vector<DepositOnlyAccount*> depositOnlyAccount;

    public:
    BankClient (vector<WithdrawableAccount*> withdrawableAccounts, vector<DepositOnlyAccount*> depositOnlyAccount) {
        this->withdrawableAccounts = withdrawableAccounts;
        this->depositOnlyAccount = depositOnlyAccount;
    }

    void processTransaction() {
        for(WithdrawableAccount* acc : withdrawableAccounts) {
            acc->deposit(1000);
            acc->withdraw(500);
        }
        for(DepositOnlyAccount* acc : depositOnlyAccount) {
            acc->deposit(100);
        }
    }
};

int main () {
    vector<WithdrawableAccount*> withdrawableAccounts;
    withdrawableAccounts.push_back(new SavingAccount());
    withdrawableAccounts.push_back(new CurrentAccount());

    vector<DepositOnlyAccount*> depositOnlyAccount;
    depositOnlyAccount.push_back(new FDAccount());

    BankClient* client = new BankClient(withdrawableAccounts, depositOnlyAccount);

    client->processTransaction();
}