#include<iostream>
#include<string>
using namespace std;

// Simple Factory Pattern is a design pattern that provides a static method to create objects without exposing the instantiation logic to the client. 
// It allows you to create objects based on a given input, such as a string or an enum, and returns an instance of the appropriate class.

// Product Interface
class Product {
public: 
    virtual void use() = 0; // Pure virtual function
    virtual ~Product() {} // Virtual destructor for proper cleanup
};

// Concrete Product A
class ProductA: public Product {
    public:
        void use() override {
            cout << "Using Product A" << endl;
        }
};

// Concrete Product B
class ProductB: public Product {
    public:
        void use() override {
            cout << "Using Product B" << endl;
        }
};

// Simple Factory 
class SimpleFactory {
    public:
        // Static method to create products based on the type
        static Product* createProduct(string type) {
            if (type == "A") {
                return new ProductA();
            } else if (type == "B") {
                return new ProductB();
            } else {
                return nullptr; // Return nullptr for unknown product types
            }  
        }   
};

int main() {
    string productType = "A";
    SimpleFactory* factory = new SimpleFactory(); // Create the factory
    Product* product = factory->createProduct(productType); // Create a product based on the type
    if (product) {
        product->use(); // Use the product
        delete product; // Clean up
    } else {
        cout << "Unknown product type: " << productType << endl;
    }
    delete factory; // Clean up factory
    return 0;
}