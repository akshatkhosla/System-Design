#include<iostream>
#include<memory>
#include<string>

// Facade Design Pattern provides a simplified interface to a complex subsystem. 
// In this example, we have a HomeTheatre system with multiple components (Amplifier, DVDPlayer, Projector, etc.). 
// The HomeTheatreFacade class provides a simple interface to control the entire system without exposing the complexities of each component.

class Amplifier {
public: 
    void on() { std::cout << "Amplifier on\n"; }
    void off() { std::cout << "Amplifier off\n"; }
    void setVolume(int level) { std::cout << "Amplifier volume set to " << level << "\n"; }
};

class Projector {
public:
    void on() { std::cout << "Projector on\n"; }
    void setInput(const std::string& input) { std::cout << "Projector input set to " << input << "\n"; }
    void off() { std::cout << "Projector off\n"; }
};

class StreamingPlayer {
public:
    void on() { std::cout << "Streaming Player on\n"; }
    void play(const std::string& movie) { std::cout << "Streaming Player playing \"" << movie << "\"\n"; }
    void stop() { std::cout << "Streaming Player stopped\n"; }
    void off() { std::cout << "Streaming Player off\n"; }
};

class Lights {
public:
    void dim(int level) { std::cout << "Lights dimmed to " << level << "%\n"; }
    void on() { std::cout << "Lights on\n"; }
};

// Facade class that simplifies the interface to the home theatre system
class HomeTheatreFacade {
private:
        Amplifier amp_;
        Projector projector_;
        StreamingPlayer player_;
        Lights lights_;
public:
    HomeTheatreFacade(Amplifier& a, Projector& p, StreamingPlayer& sp, Lights& l)
        : amp_(a), projector_(p), player_(sp), lights_(l) {}

    void watchMovie(const std::string& movie) {
        lights_.dim(10);
        amp_.on();
        amp_.setVolume(5);
        projector_.on();
        projector_.setInput("DVD");
        player_.on();
        player_.play(movie);
    }
    void endMovie() {
        lights_.on();
        amp_.off();
        projector_.off();
        player_.stop();
        player_.off();
    }
};

int main() {
    Amplifier amp;
    Projector projector;
    StreamingPlayer player;
    Lights lights;

    HomeTheatreFacade homeTheatre(amp, projector, player, lights);
    homeTheatre.watchMovie("Inception");
    std::cout << "\n--- Movie is playing ---\n\n";
    homeTheatre.endMovie();

    return 0;
}