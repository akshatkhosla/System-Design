#include<iostream>
#include<vector>
#include<unordered_map>
#include<queue>
#include<set>
#include<algorithm>
#include<random>
using namespace std;

class BoardTransformation  {
public:
    virtual int apply(int position) = 0;
    virtual int triggerSquare() = 0;
};

class Snake : public BoardTransformation {
private:
    int head_;
    int tail_;
public:
    Snake(int head, int tail) : head_(head), tail_(tail) {}
    int apply(int position) override {
        return tail_;
    }
    int triggerSquare() override {
        return head_;
    }
};

class Ladder : public BoardTransformation {
private:
    int base_;
    int top_;
public:
    Ladder(int base, int top) : base_(base), top_(top) {}
    int apply(int position) override {
        return top_;
    }
    int triggerSquare() override {
        return base_;
    }
};

class Board {
private: 
    int totalSquares_;
    unordered_map<int, BoardTransformation*> transformations_; 
public:
    Board(int totalSquares) : totalSquares_(totalSquares) {}
    
    int resolvePosition(int currPosition, int roll) {
        int newPosition = currPosition + roll;
        if (newPosition > totalSquares_) {
            return currPosition; // Stay in place if overshooting
        }
        while (transformations_.count(newPosition)) {
            newPosition = transformations_[newPosition]->apply(newPosition);
        }
        return newPosition; 
    }

    void addTransformation(BoardTransformation* transformation) {
        transformations_[transformation->triggerSquare()] = transformation;
    }
    bool isWinningPosition(int position) {
        return position == totalSquares_;
    }
    int getTotalSquares() const { return totalSquares_; }
};

class DiceStrategy {
    public:
    virtual int roll() = 0;
};

class RandomDice : public DiceStrategy {
private:
    int numDice_;
    mt19937 rng_;
public:
    RandomDice(int numDice) : numDice_(numDice), rng_(random_device{}()) {}
    
    int roll() override {
        uniform_int_distribution<int> dist(1, 6); 
        int totalRoll = 0;
        for (int i = 0; i < numDice_; ++i) {
            totalRoll += dist(rng_);
        }
        return totalRoll;
    }
};

class FixedDice : public DiceStrategy {
private:
    vector<int> sequence_;
    int currentRollIndex_;
public:
    FixedDice(const vector<int>& sequence) : sequence_(sequence), currentRollIndex_(0) {}
    int roll() override {
        return sequence_[currentRollIndex_++%sequence_.size()];
    }
};

class Player {
private:
    string name_;
    int position_;
public:
    Player(const string& name) : name_(name), position_(0) {}
    const string& getName() const {
        return name_;
    }
    int getPosition() const {
        return position_;
    }
    void setPosition(int position) {
        position_ = position;
    }
};

class TurnResult {
public:
    string player;
    int roll;
    int startPosition;
    int endPosition;
    bool hitTransformation; // Indicates if the player hit a snake or ladder
    TurnResult(const string& player, int r, int start, int end, bool hit) : 
                    player(player), roll(r), startPosition(start), endPosition(end), hitTransformation(hit) {}
};

class GameConfig {
public:
    int totalSquares = 100;
    int numDice = 1;
    int startingPosition = 0;
    vector<string> playerNames;
    unordered_map<int, int> snakes; // head -> tail
    unordered_map<int, int> ladders; // base -> top
};

class Game {
private:
    Board board_;
    DiceStrategy* dice_;
    deque<Player> players_;
    vector<TurnResult> turnResults_;
    bool gameOver_;

    // Executes one turn for the front-of-queue player: rolls dice, resolves board position
    // (applying any snake/ladder), and returns a TurnResult capturing what happened.
    // rawPosition vs newPosition comparison detects transformation hits while excluding overshoots.
    TurnResult playTurn() {
        Player& currentPlayer = players_.front();
        int roll = dice_->roll();
        int startPosition = currentPlayer.getPosition();
        int newPosition = board_.resolvePosition(startPosition, roll);

        int rawPosition = startPosition + roll;
        // true if player moved to a different square and that square had a snake or ladder (not just overshooting)
        bool hitTransformation = (rawPosition <= board_.getTotalSquares()) && (rawPosition != newPosition);

        currentPlayer.setPosition(newPosition);
        return TurnResult(currentPlayer.getName(), roll, startPosition, newPosition, hitTransformation);
    }

    // Rotates the player queue so the next player becomes current.
    // Moves front player to the back, maintaining round-robin order.
    void advanceTurn() {
        players_.push_back(players_.front());
        players_.pop_front();
    }

public:
    // Builds the board and populates players, snakes, and ladders from config.
    Game(const GameConfig& config, DiceStrategy* dice) : board_(config.totalSquares), dice_(dice), gameOver_(false) {
        for (const auto& name : config.playerNames) {
            players_.emplace_back(name);
        }
        for (const auto& [head, tail] : config.snakes) {
            board_.addTransformation(new Snake(head, tail));
        }
        for (const auto& [base, top] : config.ladders) {
            board_.addTransformation(new Ladder(base, top));
        }
    }

    // Returns player names in their current turn order (front of queue first).
    vector<string> getTurnOrder() const {
        vector<string> order;
        for (const auto& p : players_) {
            order.push_back(p.getName());
        }
        return order;
    }

    // Returns a snapshot of every player's current board position keyed by name.
    unordered_map<string, int> getPositions() const {
        unordered_map<string, int> positions;
        for (const auto& p : players_) {
            positions[p.getName()] = p.getPosition();
        }
        return positions;
    }

    // Main game loop: repeatedly calls playTurn() and advanceTurn() until a player
    // reaches the final square. Logs each turn and declares the winner on exit.
    void startGame() {
        while (!gameOver_) {
            TurnResult result = playTurn();
            turnResults_.push_back(result);

            cout << result.player << " rolled " << result.roll
                 << " | " << result.startPosition << " -> " << result.endPosition;
            if (result.hitTransformation) cout << " [snake/ladder]";
            cout << "\n";

            if (board_.isWinningPosition(result.endPosition)) {
                cout << result.player << " wins!\n";
                gameOver_ = true;
            } else {
                advanceTurn();
            }
        }
    }
};

int main() {
    GameConfig config;
    config.totalSquares = 100;
    config.numDice = 1;
    config.playerNames = {"Alice", "Bob"};
    config.snakes = {{16, 6}, {48, 26}, {49, 11}, {56, 53}, {62, 19}, {64, 60}, {87, 24}, {93, 73}, {95, 75}, {98, 78}};
    config.ladders = {{1, 38}, {4, 14}, {9, 31}, {21, 42}, {28, 84}, {36, 44}, {51, 67}, {71, 91}, {80, 100}};

    RandomDice dice(config.numDice);
    Game game(config, &dice);
    cout << "Turn order: ";
    for (const auto& name : game.getTurnOrder()) {
        cout << name << " ";
    }
    cout << "\nStarting game...\n";
    game.startGame();
    return 0;
}