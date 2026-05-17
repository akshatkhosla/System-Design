#include<iostream>
#include<string>
using namespace std;

//Interface Segregation Principle (ISP) states that clients should not be forced to depend on interfaces they do not use. 
// It promotes the idea of creating specific interfaces for different functionalities, rather than a single, large interface that encompasses multiple functionalities.

class TwoDimensionalShape {
    public:
    virtual double area() = 0;
};

class ThreeDimensionalShape {
    public:
    virtual double volume() = 0;
    virtual double area() = 0;
};

class Square : public TwoDimensionalShape {
    private:
    double side;
    public:
    Square(double side) {
        this->side = side;
    }
    double area() override {
        return side * side;
    }
};

class Cube : public ThreeDimensionalShape {
    private:
    double side;
    public:
    Cube(double side) {
        this->side = side;
    }
    double volume() override {
        return side * side * side;
    }
    double area() override {
        return 6 * side * side;
    }
};

int main() {
    Square* square = new Square(5);
    cout << "Area of Square: " << square->area() << endl;

    Cube* cube = new Cube(3);
    cout << "Volume of Cube: " << cube->volume() << endl;
    cout << "Surface Area of Cube: " << cube->area() << endl;

    return 0;
}
