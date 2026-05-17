#include <iostream>
#include <vector>
using namespace std;

// Dependency Inversion Principle (DIP) stated that high-level modules should not depend on low . Both should depend on abstractions.
// Abstractions should not depend on details. Details should depend on abstractions.
// In this example, we have a userService class that depends on a Persistance interface. 
// The userService class can work with any implementation of the Persistance interface, such as MySQLPersistance or MongoDBPersistance. 
// This allows for flexibility and decoupling between the userService and the specific database implementations.
class Persistance {
    public:
    virtual void save() = 0;
};

class MySQLPersistance : public Persistance {
    public:
    void save() override {
        cout << "Saving data to MySQL database..." << endl;
    }
};

class MonogoDBPersistance : public Persistance {
    public:
    void save() override {
        cout << "Saving data to MongoDB database..." << endl;
    }
};

class userService {
    private:
    Persistance* persistance; // Dependency on abstraction (interface) rather than concrete implementation
    public:
    // Constructor injection to inject the dependency
    userService(Persistance* persistance) {
        this->persistance = persistance;
    }
    void saveUserData() {
        persistance->save();
    }
};

int main() {
    MySQLPersistance* mysqlPersistance = new MySQLPersistance();
    userService* userService1 = new userService(mysqlPersistance);
    userService1->saveUserData();

    MonogoDBPersistance* mongoDBPersistance = new MonogoDBPersistance();
    userService* userService2 = new userService(mongoDBPersistance);
    userService2->saveUserData();

    return 0;
}