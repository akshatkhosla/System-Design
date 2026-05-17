#include<iostream>
#include<vector>

using namespace std;

class Product {
    public:
    string name;
    double price;

    Product(string name, double price) {
        this->name = name;
        this->price = price;
    }
};

// Responsible for a single functionality.
class ShoppingCart {
    public:
    vector<Product*> products;

    void addProduct(Product* p) {
        products.push_back(p);
    }
    vector<Product*> getProducts() {
        return products;
    }

    double calculateTotPrice() {
        double total = 0;
        for(auto p: products) {
            total += p->price;
        }
        return total;
    }
};

// ShoppingCartPrinter: Only responsible for printing invoices
class ShoppingCartPrinter {
    private:
    ShoppingCart* cart;
    public:
    ShoppingCartPrinter (ShoppingCart* cart) {
        this->cart = cart;
    }
    void printInvoice() {
        cout << "Shopping cart Invoice" << endl;
        for(auto p : cart->getProducts()) {
            cout << p->name << " - Rs. " << p->price << endl;
        }
        cout << "Total: Rs - " << cart->calculateTotPrice() << endl;
    }

};

// ShoppingCartStorage: Only responsible for saving cart to DB
class ShoppingCartStorage {
private:
    ShoppingCart* cart; 

public:
    ShoppingCartStorage(ShoppingCart* cart) { 
        this->cart = cart; 
    }

    void saveToDatabase() {
        cout << "Saving shopping cart to database..." << endl;
    }
};

int main()
{
    ShoppingCart* cart = new ShoppingCart();

    cart->addProduct(new Product("Laptop", 50000));
    cart->addProduct(new Product("Mouse", 2000));

    ShoppingCartPrinter* printer = new ShoppingCartPrinter(cart);
    printer->printInvoice();

    ShoppingCartStorage* storage = new ShoppingCartStorage(cart);
    storage->saveToDatabase();
    return 0;
}