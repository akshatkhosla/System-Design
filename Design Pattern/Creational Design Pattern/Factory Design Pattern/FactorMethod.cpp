#include<iostream>
#include<string>
using namespace std;

// Factory Method Pattern defines an interface for creating an object, but lets subclasses decide which class to instantiate.
// Key difference from Simple Factory: the *creator* (base class) calls createNotification() in its own notify() method —
// subclasses override createNotification() to swap the product. This is the Template Method + Factory Method combo.
// Follows Open/Closed Principle: add a new channel (WhatsApp) by adding a new factory subclass, zero changes to existing code.

// ─────────────────────────────────────────────
//  PRODUCT interface
// ─────────────────────────────────────────────

class Notification {
    public:
        // send() takes the actual message content — every channel must know how to dispatch it
        virtual void send(const string& message) = 0;
        virtual ~Notification() {}
};

// ─────────────────────────────────────────────
//  CONCRETE PRODUCTS
// ─────────────────────────────────────────────

class EmailNotification: public Notification {
    string recipientEmail;
    public:
        EmailNotification(const string& email) : recipientEmail(email) {}
        void send(const string& message) override {
            cout << "[EMAIL] To: " << recipientEmail << " | Msg: " << message << endl;
        }
};

class SMSNotification: public Notification {
    string phoneNumber;
    public:
        SMSNotification(const string& phone) : phoneNumber(phone) {}
        void send(const string& message) override {
            cout << "[SMS]   To: " << phoneNumber << " | Msg: " << message << endl;
        }
};

class PushNotification: public Notification {
    string deviceId;
    public:
        PushNotification(const string& id) : deviceId(id) {}
        void send(const string& message) override {
            cout << "[PUSH]  DeviceID: " << deviceId << " | Msg: " << message << endl;
        }
};

// ─────────────────────────────────────────────
//  CREATOR (abstract)
// ─────────────────────────────────────────────

class NotificationFactory {
    public:
        // The factory method — subclasses override this to instantiate the right product.
        // Protected because client code should call notify(), not createNotification() directly.
        virtual Notification* createNotification() = 0;

        // Template method: contains the sending workflow.
        // Calls createNotification() internally — this is what makes it Factory Method, not just a virtual constructor.
        void notify(const string& message) {
            Notification* n = createNotification();
            n->send(message);
            delete n;
        }
        virtual ~NotificationFactory() {}
};

// ─────────────────────────────────────────────
//  CONCRETE CREATORS
// ─────────────────────────────────────────────

class EmailNotificationFactory: public NotificationFactory {
    string email;
    public:
        EmailNotificationFactory(const string& email) : email(email) {}
        Notification* createNotification() override {
            return new EmailNotification(email);
        }
};

class SMSNotificationFactory: public NotificationFactory {
    string phone;
    public:
        SMSNotificationFactory(const string& phone) : phone(phone) {}
        Notification* createNotification() override {
            return new SMSNotification(phone);
        }
};

class PushNotificationFactory: public NotificationFactory {
    string deviceId;
    public:
        PushNotificationFactory(const string& id) : deviceId(id) {}
        Notification* createNotification() override {
            return new PushNotification(deviceId);
        }
};

// ─────────────────────────────────────────────
//  CLIENT CODE
// ─────────────────────────────────────────────

// Client only depends on NotificationFactory* — it never mentions Email/SMS/Push directly.
// Swap the factory pointer to change the channel: zero client code changes needed.
void sendAlert(NotificationFactory* factory, const string& message) {
    factory->notify(message);
}

int main() {
    NotificationFactory* factories[] = {
        new EmailNotificationFactory("user@example.com"),
        new SMSNotificationFactory("+1-800-555-0199"),
        new PushNotificationFactory("device-xyz-9821")
    };

    string alert = "Your OTP is 482910";

    // Send the same alert through all channels — client code is blissfully unaware of the concrete products.
    for (NotificationFactory* factory : factories) {
        sendAlert(factory, alert);
        delete factory;
    }

    return 0;
}