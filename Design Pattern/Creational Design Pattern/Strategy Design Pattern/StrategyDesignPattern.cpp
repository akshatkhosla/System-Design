#include<iostream>
#include<string>
using namespace std;

// Strategy Design Pattern: defines a family of algorithms (behaviors), encapsulates each one,
// and makes them interchangeable at runtime without changing the context (Robot) class.

// --- Strategy Interfaces ---
// Each interface represents one axis of behavior that can vary independently.

class TalkableRobot {
public:
    virtual void talk() = 0;    // pure virtual — enforces implementation in concrete strategies
    virtual ~TalkableRobot() = default;
};

class WalkableRobot {
public:
    virtual void walk() = 0;    // pure virtual — enforces implementation in concrete strategies
    virtual ~WalkableRobot() = default;
};

class FlyableRobot {
public:
    virtual void fly() = 0;
    virtual ~FlyableRobot() = default;
};

// --- Concrete Talk Strategies ---

class NormalTalk : public TalkableRobot {
public:
    void talk() override {
        cout << "Talking normally." << endl;
    }
};

class NoTalk : public TalkableRobot {
public:
    void talk() override {
        cout << "Cannot talk." << endl;
    }
};

// --- Concrete Walk Strategies ---

class NormalWalk : public WalkableRobot {
public:
    void walk() override {
        cout << "Walking normally." << endl;
    }
};

class NoWalk : public WalkableRobot {
public:
    void walk() override {
        cout << "Cannot walk." << endl;
    }
};

// --- Concrete Fly Strategies ---

class NormalFly : public FlyableRobot {
public:
    void fly() override {
        cout << "Flying normally." << endl;
    }
};

class NoFly : public FlyableRobot {
public:
    void fly() override {
        cout << "Cannot fly." << endl;
    }
};

// --- Context Class ---
// Robot holds references to strategy objects and delegates behavior to them.
// Behaviors can be swapped at runtime via setters without modifying this class.
class Robot {
private:
    WalkableRobot* walkBehavior;
    TalkableRobot* talkBehavior;
    FlyableRobot*  flyBehavior;

public:
    Robot(WalkableRobot* w, TalkableRobot* t, FlyableRobot* f)
        : walkBehavior(w), talkBehavior(t), flyBehavior(f) {}

    // Runtime strategy swap — core benefit of the Strategy pattern
    void setWalkBehavior(WalkableRobot* w) { walkBehavior = w; }
    void setTalkBehavior(TalkableRobot* t) { talkBehavior = t; }
    void setFlyBehavior(FlyableRobot* f)   { flyBehavior  = f; }

    
    void performWalk() { walkBehavior->walk(); }
    void performTalk() { talkBehavior->talk(); }
    void performFly()  { flyBehavior->fly();   }

    // Destructor owns and cleans up the strategy objects
    ~Robot() {
        delete walkBehavior;
        delete talkBehavior;
        delete flyBehavior;
    }
};

int main() {
    // Robot 1: can walk and talk, but cannot fly
    Robot* robot1 = new Robot(new NormalWalk(), new NormalTalk(), new NoFly());
    robot1->performWalk();
    robot1->performTalk();
    robot1->performFly();

    cout << "--------------------" << endl;

    // Robot 2: can fly, but cannot walk or talk — different strategy combination
    Robot* robot2 = new Robot(new NoWalk(), new NoTalk(), new NormalFly());
    robot2->performWalk();
    robot2->performTalk();
    robot2->performFly();

    cout << "--------------------" << endl;

    // Swapping strategy at runtime — robot1 gains flight capability
    robot1->setFlyBehavior(new NormalFly());
    robot1->performFly();

    delete robot1;
    delete robot2;

    return 0;
}
