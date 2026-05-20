#include<iostream>
using namespace std;

// Strategy design pattern states that we can define a family of algorithms, encapsulate each one, and make them interchangeable.
class WalkableRobot {
    public: 
    virtual void walk();
};

class NormalWalk : public WalkableRobot {
    public:
    void walk () override {
        cout << "Walking normally " << endl;
    }
};

class NoWalk : public WalkableRobot {
    public:
    void walk () override {
        cout << " Cannot walk " << endl;
    }
};

class TalkableRobot {
    public: 
    virtual void talk();
};

class TalkingRobot : public TalkableRobot{
    public: 
    void talk () override {
        cout << "Talking normally " << endl;
    }
};
class NoTalk : public TalkableRobot{
    public: 
    void talk () override {
        cout << "Cannot Talk " << endl;
    }
};


class FlyableRobot {
public:
    virtual void fly() = 0;
    virtual ~FlyableRobot() {}
};

class NormalFly : public FlyableRobot {
public:
    void fly() override { 
        cout << "Flying normally..." << endl; 
    }
};

class NoFly : public FlyableRobot {
public:
    void fly() override { 
        cout << "Cannot fly." << endl; 
    }
};

// --- Robot Class ---
// The Robot class uses composition to include the behaviors of walking, talking, and flying. 
// Each behavior is defined as a separate class that implements a common interface, allowing for flexibility and interchangeability
class Robot {
    private: 
    WalkableRobot* walkBehavior;
    TalkableRobot* talkBehavior;
    FlyableRobot* flyBehavior;

    public:
    Robot (WalkableRobot* w, TalkableRobot* t, FlyableRobot* f) {
        this->walkBehavior = w;
        this->talkBehavior = t;
        this->flyBehavior = f;
    }

    void walk () {
        walkBehavior->walk();
    }
    void talk () {
        talkBehavior->talk();
    }
    void fly () {
        flyBehavior->fly();
    }

    virtual void projection () = 0;
};

class CompanionRobot : public Robot {
public:
    CompanionRobot(WalkableRobot* w, TalkableRobot* t, FlyableRobot* f)
        : Robot(w, t, f) {}

    void projection() override {
        cout << "Displaying friendly companion features..." << endl;
    }
};

class WorkerRobot : public Robot {
public:
    WorkerRobot(WalkableRobot* w, TalkableRobot* t, FlyableRobot* f)
        : Robot(w, t, f) {}

    void projection() override {
        cout << "Displaying worker efficiency stats..." << endl;
    }
};

// --- Main Function ---
int main() {
    Robot *robot1 = new CompanionRobot(new NormalWalk(), new TalkingRobot(), new NoFly());
    robot1->walk();
    robot1->talk();
    robot1->fly();
    robot1->projection();
    cout << "--------------------" << endl;

    Robot *robot2 = new WorkerRobot(new NoWalk(), new NoTalk(), new NormalFly());
    robot2->walk();
    robot2->talk();
    robot2->fly();
    robot2->projection();

    return 0;
}