#include<iostream>
#include<vector>

using namespace std;

class Product {
    public:
    string name;
    double price;

    Product (string name, double price)
    {
        this->name = name;
        this->price = price;
    }
};

class ShoppingCart{
    private:
    vector<Product*> products;
    public: 

    void addProduct(Product* p) {
        products.push_back(p);
    }

    const vector<Product*> getProducts() {
        return products;
    }

    // 1. Calculate Total Price
    double calculateTotPrice() {
        double total = 0;
        for(auto p : products) {
            total += p->price;
        }
        return total;
    }

    // 2. Voilation SRP - Print invoice function (Shoule be a sepearte class)
    void printInvoice() {
        cout << " Shopping cart Invoice: " << endl;
        for(auto p: products) {
            cout << p->name << " - Rs. " << p->price << endl;
        }
        cout << "Total: Rs. " << calculateTotPrice() << endl;
    }
    

    // 3. Voilation SRP - Save to DB
    void saveToDatabase () {
        cout << "Saving shopping cart data to DB.." << endl;
    }
};

int main() {
    ShoppingCart* cart = new ShoppingCart();
    cart->addProduct(new Product("Laptop", 50000));
    cart->addProduct(new Product("Mouse", 2000));

    cart->printInvoice();
    cart->saveToDatabase();
    return 0;
}