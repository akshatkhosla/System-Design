#include<iostream>
#include<vector>

using namespace std;

class Product {
    public:
    string name;
    double price;

    Product(string name, double price)
    {
        this->name = name;
        this->price = price;
    }
};

class ShoppingCart {
    private:
    vector<Product*> products;

    public:
    void addProduct(Product* p) {
        products.push_back(p);
    }

    const vector<Product*> getProduct() {
        return products;
    }

    double calculateTotPrice() {
        double total = 0;
        for(auto p : products) {
            total += p->price;
        }
        return total;
    }
};

class ShoppingCartPrinter {
    private: 
    ShoppingCart* cart;

    public: 
    ShoppingCartPrinter(ShoppingCart* cart) {
        this->cart = cart;
    }

    void printInvoice() {
        cout << "Shopping Cart Invoice:\n";
        for (auto p : cart->getProduct()) {
            cout << p->name << " - Rs " << p->price << endl;
        }
        cout << "Total: Rs " << cart->calculateTotPrice() << endl;
    }

};

//ShoppingCartStorage: Only responsible for saving cart to DB
// Voilation of OCP cause all the functions to save are defined in single class.
class DBPersistance {
private:
    ShoppingCart* cart; 

public:
    virtual void save(ShoppingCart* cart) = 0;
};

class saveToDatabase : public DBPersistance {
    public:
    void save(ShoppingCart* cart) override {
        cout << "Saving shopping cart to DB..." << endl;
    }
};  

class saveToMongo : public DBPersistance {
    public:
    void save(ShoppingCart* cart) override {
        cout << "Saving shopping cart to MonogoDB..."<< endl;
    }
};  

class saveToFile : public DBPersistance {
    public:
    void save(ShoppingCart* cart) override {
        cout << "Saving shopping cart to File System..."<< endl;
    }
};  

int main()
{
    ShoppingCart* cart = new ShoppingCart();

    cart->addProduct(new Product("Laptop", 50000));
    cart->addProduct(new Product("Mouse", 2000));

    ShoppingCartPrinter* printer = new ShoppingCartPrinter(cart);
    printer->printInvoice();

    DBPersistance* db = new saveToDatabase();
    DBPersistance* mongo = new saveToMongo();
    DBPersistance* file = new saveToFile();

    db->save(cart);
    mongo->save(cart);
    file->save(cart);
    
   //db->saveToSQLDatabase();
}