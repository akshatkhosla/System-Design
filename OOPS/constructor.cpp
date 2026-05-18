#include<iostream>
using namespace std;

class AbsEmployee
{
    protected:
    string name;
public:
    virtual void askForPromotion() = 0; // Pure virtual function making this an abstract class
};

// Employee class inherits from AbsEmployee and implements the askForPromotion function
class Employee : public AbsEmployee
{
private:
    string name;
    string company;
    int age;
public:
    void introduceYourself()
    {
        cout << "Name - " << name << endl;
        cout << "Company - " << company << endl;
        cout << "Age - " << age << endl;
    }
    // Implementation of the pure virtual function from AbsEmployee
    void askForPromotion()
    {
        if (age >= 30)
            cout << name << " got promoted!" << endl;
        else
            cout << name << ", sorry no promotion for you!" << endl;
    }

    Employee(string name, string company, int age)
    {
        this->name = name;
        this->company = company;
        this->age = age;
    }
};

int main()
{
    Employee employee1 = Employee("John", "Google", 30);
    // employee1.introduceYourself();
    // employee1.name = "John Doe"; // This will cause an error because 'name' is private and cannot be accessed directly from outside the class.
    employee1.askForPromotion();
    Employee employee2 = Employee("Alice", "Microsoft", 28);
    // employee2.introduceYourself();
    employee2.askForPromotion();

    return 0;
}