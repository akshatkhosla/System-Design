#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

// Observer Pattern: a Subject maintains a list of Observers and notifies them
// automatically whenever its state changes.

// Observer interface — receives update notifications from the subject.
class ISubscriber {
public:
    virtual void update(const string& videoTitle) = 0;
    virtual ~ISubscriber() = default;
};

// Subject interface — manages and notifies observers.
class IChannel {
public:
    virtual void subscribe(ISubscriber* subscriber) = 0;
    virtual void unsubscribe(ISubscriber* subscriber) = 0;
    virtual void notifySubscribers() = 0;
    virtual ~IChannel() = default;
};



// Concrete Subject — holds subscriber list and broadcasts upload events.
class YouTubeChannel : public IChannel {
private:
    vector<ISubscriber*> subscribers;
    string channelName;
    string latestVideoTitle;
public:
    YouTubeChannel(const string& name) : channelName(name) {}
    void notifySubscribers() override {
        for(ISubscriber* subscriber : subscribers) {
            subscriber->update(latestVideoTitle);
        }
    }
    void subscribe(ISubscriber* subscriber) override {
        if(find(subscribers.begin(), subscribers.end(), subscriber) == subscribers.end()) {
            subscribers.push_back(subscriber);
        }
    }
    void unsubscribe(ISubscriber* subscriber) override {
        subscribers.erase(remove(subscribers.begin(), subscribers.end(), subscriber), subscribers.end());
    }
    void uploadVideo(const string& videoTitle) {
        latestVideoTitle = videoTitle;
        cout << "New video uploaded: " << videoTitle << endl;
        notifySubscribers();
    }
    void getVideoInfo() {
        cout << "Channel: " << channelName << ", Latest Video: " << latestVideoTitle << endl;
    }
    string getChannelName() {
        return channelName;
    }
};

// Concrete Observer — prints a notification when the watched channel uploads a video.
class Subscriber : public ISubscriber {
private: 
    string name;
    YouTubeChannel* channel;
public:
    Subscriber(const string& name, YouTubeChannel* channel){
        this->name = name;
        this->channel = channel;
    }
    void update(const string& videoTitle) override {
        cout << name << " received notification: New video titled '" << videoTitle << "' uploaded on " << channel->getChannelName() << endl;
    }
};

int main() {
    YouTubeChannel* channel = new YouTubeChannel("Tech Reviews");
    Subscriber* subscriber1 = new Subscriber("Alice", channel);
    Subscriber* subscriber2 = new Subscriber("Bob", channel);

    channel->subscribe(subscriber1);
    channel->subscribe(subscriber2);

    channel->uploadVideo("Latest Smartphone Review");
    channel->uploadVideo("Top 5 Laptops of 2024");

    channel->unsubscribe(subscriber1);
    channel->uploadVideo("Best Budget Gadgets");

    delete subscriber1;
    delete subscriber2;
    delete channel;

    return 0;
}