#include<iostream>
#include<string>
using namespace std;

// Abstract Factory Pattern provides an interface for creating families of related or dependent objects 
// without specifying their concrete classes.

// CROSS PLATFORM UI

// Product Interfaces
class Button {
    public: 
    virtual void render() = 0;  
    virtual void onClick() = 0;
    virtual ~Button() {}
};

class Checkbox {
    public: 
    virtual void render() = 0;  
    virtual void toggle() = 0;
    virtual ~Checkbox() {}
};

// Concrete Products
class WindowsButton: public Button {
    public: 
    void render() override {
        cout << "Rendering a Windows-style button." << endl;
    }
    void onClick() override {
        cout << "Windows button clicked!" << endl;
    }
};

class MacButton : public Button {
    public:
    void render() override {
        cout << "Rendering a Mac-style button." << endl;
    }
    void onClick() override {
        cout << "Mac button clicked!" << endl;
    }
};

class WindowsCheckbox: public Checkbox {
    public: 
    void render() override {
        cout << "Rendering a Windows-style checkbox." << endl;
    }
    void toggle() override {
        cout << "Windows checkbox toggled!" << endl;
    }
};

class MacCheckbox : public Checkbox {
    public:
    void render() override {
        cout << "Rendering a Mac-style checkbox." << endl;
    }
    void toggle() override {
        cout << "Mac checkbox toggled!" << endl;
    }
};


// Abstract Factory Interface
class GUIFactory {
    public:
    // Factory methods return the product — client decides when/how to use it
    virtual Button*    createButton()   = 0;
    virtual Checkbox*  createCheckbox() = 0;
    virtual ~GUIFactory() {}
};

// Concrete Factories
class WindowsFactory : public GUIFactory {
    public:
    Button* createButton() override {
        return new WindowsButton();    // create and return only — no render/click here
    }
    Checkbox* createCheckbox() override {
        return new WindowsCheckbox();
    }
};

class MacFactory : public GUIFactory {
    public:
    Button* createButton() override {
        return new MacButton();
    }
    Checkbox* createCheckbox() override {
        return new MacCheckbox();
    }
};

// ─────────────────────────────────────────────
//  CLIENT
// ─────────────────────────────────────────────

// Application holds the factory by aggregation (injected, doesn't own lifetime)
// and owns Button/Checkbox by composition (creates them, destroys them in destructor)
class Application {
    private:
    GUIFactory* factory;   // aggregation — caller owns the factory
    Button*     button;    // composition — Application owns this
    Checkbox*   checkbox;  // composition — Application owns this

    public:
        // Application constructor takes a factory — this is Dependency Injection (DI)
        // Explain this line of code - it initializes the Application with a specific GUIFactory, 
        // allowing it to create products (buttons and checkboxes) without hardcoding their types. This promotes flexibility and decouples the Application from concrete product implementations.
        Application(GUIFactory* f) {
            this->factory = f;
            this->button = nullptr;
            this->checkbox = nullptr;
        }

        void buildUI() {
            button   = factory->createButton();
            checkbox = factory->createCheckbox();
        }

        void render() {
            button->render();
            checkbox->render();
        }

        void interact() {
            button->onClick();
            checkbox->toggle();
        }

        ~Application() {
            delete button;
            delete checkbox;
            // factory is NOT deleted here — Application doesn't own it (aggregation)
        }
};

// Selects the right factory based on OS — keeps main() clean
GUIFactory* createFactory(const string& osType) {
    if (osType == "Windows") return new WindowsFactory();
    if (osType == "Mac")     return new MacFactory();
    return nullptr;
}

int main() {
    string osType = "Windows"; // determined at runtime (config, env var, etc.)

    GUIFactory* factory = createFactory(osType);
    if (!factory) {
        cout << "Unsupported OS: " << osType << endl;
        return 1;
    }

    Application app(factory);
    app.buildUI();
    app.render();
    app.interact();

    delete factory;
    return 0;
}