/*
Pseudo Code Reference:
1. Core Logic
2. Edge cases

important function - Game.makeMove(), board.placeDisc(), board.checkWin()

class Game {
    makeMove(currentPlayer, column) -> bool

    Core Logic:
    // placeDisc
    // Check if player won
    // if not check if draw -> board full
    // else switch turn

    Edge Cases:
    // game already over
    // wrong player turn

    if state != IN_PROGRESS
        return false
    if player != currentPlayer
        return false

    Board* board
    board.placeDisc(column)
    if (checkWin(row, col, player.getColor()))
        state = WIN
        winner = player
    else if board.isFull()
        state = DRAW
    else
        currentPlayer = (player == player1) ? player2 : player1

    return true
};

class Board{
    placeDisc(column, color){
        //Core Logic:
        1. find the lowest empty row for that column
        2. place disc
        3. return the row it landed in
        // Edge Cases
        1. Column is not out of bound
        2. Column is full

        if column < 0 || column >= board.getCol()
            return -1;
        if(!board.canPlace())
            return -1

        for row = row - 1 to 0
            if(grid[row][column] == NONE)
                grid[row][column] = color
                return row

        return -1;
    }
    checkWin(row, column, color){
        //Core Logic
        1. check for four in a row in all four directions
        2. return true if found

        // edge cases
        1. row and col should not be out of bound
        2. cell at (row, col) does not match the color -> return false
    }

    directions = {
        [0,1],  // horizontal
        [1,0],  // vertical
        [1,1],  // diagonal (\)
        [-1,1]  // diagonal (/)
    }

    for dr, dc in directions
        count = 1  // the placed disc itself
        count += countInDirection(row, col,  dr,  dc, color)
        count += countInDirection(row, col, -dr, -dc, color)
        if(count >= 4) return true

    return false

    int countInDirection(row, col, dr, dc, color)
        count = 0;
        r = row + dr;
        c = col  + dc;

        while(r >= 0 && r < rows && c >= 0 && c < cols && grid[r][c] == color)
            count++;
            r += dr;
            c += dc;
        return count;
}
*/

#include <iostream>
#include <vector>
#include <string>

// ─────────────────────────────────────────────
// Enums
// ─────────────────────────────────────────────

enum class DiscColor {
    NONE,
    RED,
    BLUE
};

enum class GameState {
    IN_PROGRESS,
    WON,
    DRAW
};

// ─────────────────────────────────────────────
// Player
// ─────────────────────────────────────────────

class Player {
private:
    std::string name;
    DiscColor   color;

public:
    Player(const std::string& name, DiscColor color)
        : name(name), color(color) {}

    std::string getName()  const { return name; }
    DiscColor   getColor() const { return color; }
};

// ─────────────────────────────────────────────
// Board  (6 rows × 7 columns by default)
// ─────────────────────────────────────────────

class Board {
private:
    int rows;
    int cols;
    std::vector<std::vector<DiscColor>> grid;  // grid[row][col]

    // Count consecutive same-colour discs starting from (row,col) in direction (dr,dc)
    int countInDirection(int row, int col, int dr, int dc, DiscColor color) const {
        int count = 0;
        int r = row + dr;
        int c = col  + dc;
        while (r >= 0 && r < rows && c >= 0 && c < cols && grid[r][c] == color) {
            count++;
            r += dr;
            c += dc;
        }
        return count;
    }

public:
    Board(int rows = 6, int cols = 7)
        : rows(rows), cols(cols),
          grid(rows, std::vector<DiscColor>(cols, DiscColor::NONE)) {}

    // ── canPlace ─────────────────────────────
    // Returns true if the column is valid and has at least one empty cell
    bool canPlace(int col) const {
        if (col < 0 || col >= cols) return false;
        return grid[0][col] == DiscColor::NONE;  // top cell is empty
    }

    // ── placeDisc ────────────────────────────
    // Drops a disc into `col`; returns the row where it lands, or -1 on failure.
    int placeDisc(int col, DiscColor color) {
        if (!canPlace(col)) return -1;

        // Gravity: find the lowest empty row
        for (int row = rows - 1; row >= 0; row--) {
            if (grid[row][col] == DiscColor::NONE) {
                grid[row][col] = color;
                return row;
            }
        }
        return -1;  // should not reach here if canPlace passed
    }

    // ── checkWin ─────────────────────────────
    // Returns true if placing a disc at (row, col) with `color` creates 4-in-a-row
    bool checkWin(int row, int col, DiscColor color) const {
        if (row < 0 || row >= rows || col < 0 || col >= cols) return false;
        if (grid[row][col] != color) return false;

        // Four axis directions (each checked both ways)
        const int directions[4][2] = {
            { 0,  1},   // horizontal
            { 1,  0},   // vertical
            { 1,  1},   // diagonal  (\)
            {-1,  1}    // anti-diagonal (/)
        };

        for (auto& dir : directions) {
            int dr = dir[0], dc = dir[1];
            int count = 1;  // the just-placed disc
            count += countInDirection(row, col,  dr,  dc, color);
            count += countInDirection(row, col, -dr, -dc, color);
            if (count >= 4) return true;
        }
        return false;
    }

    // ── isFull ───────────────────────────────
    bool isFull() const {
        for (int c = 0; c < cols; c++)
            if (grid[0][c] == DiscColor::NONE) return false;
        return true;
    }

    int getRows() const { return rows; }
    int getCols() const { return cols; }

    // ── display ──────────────────────────────
    void display() const {
        for (int r = 0; r < rows; r++) {
            std::cout << "| ";
            for (int c = 0; c < cols; c++) {
                if      (grid[r][c] == DiscColor::RED)  std::cout << "R ";
                else if (grid[r][c] == DiscColor::BLUE) std::cout << "B ";
                else                                     std::cout << ". ";
            }
            std::cout << "|\n";
        }
        std::cout << "  ";
        for (int c = 0; c < cols; c++) std::cout << (c + 1) << " ";
        std::cout << "\n\n";
    }
};

// ─────────────────────────────────────────────
// Game
// ─────────────────────────────────────────────

class Game {
private:
    Player*   player1;
    Player*   player2;
    Player*   currentPlayer;
    Board*    board;
    GameState state;
    Player*   winner;

public:
    Game(Player* p1, Player* p2)
        : player1(p1), player2(p2), currentPlayer(p1),
          board(new Board()), state(GameState::IN_PROGRESS), winner(nullptr) {}

    ~Game() { delete board; }

    // ── makeMove ─────────────────────────────
    // `player` drops a disc into 0-indexed `col`.
    // Returns true on a valid move, false otherwise.
    bool makeMove(Player* player, int col) {
        // Edge case 1: game already over
        if (state != GameState::IN_PROGRESS) {
            std::cout << "[ERROR] Game is already over!\n";
            return false;
        }

        // Edge case 2: wrong player's turn
        if (player != currentPlayer) {
            std::cout << "[ERROR] It is not your turn, " << player->getName() << "!\n";
            return false;
        }

        // Try to place the disc
        int row = board->placeDisc(col, player->getColor());

        // Edge case 3: column full or out of range
        if (row == -1) {
            std::cout << "[ERROR] Invalid move! Column "
                      << (col + 1) << " is full or out of range.\n";
            return false;
        }

        // Check win condition
        if (board->checkWin(row, col, player->getColor())) {
            state  = GameState::WON;
            winner = player;
        }
        // Check draw condition
        else if (board->isFull()) {
            state = GameState::DRAW;
        }
        // Successful move – switch turn
        else {
            currentPlayer = (player == player1) ? player2 : player1;
        }

        return true;
    }

    Player*   getCurrentPlayer() const { return currentPlayer; }
    GameState getGameState()     const { return state; }
    Player*   getWinner()        const { return winner; }
    Board*    getBoard()         const { return board; }
};

// ─────────────────────────────────────────────
// Helper: print game result
// ─────────────────────────────────────────────

void printResult(const Game& game) {
    switch (game.getGameState()) {
        case GameState::WON:
            std::cout << "🎉 " << game.getWinner()->getName() << " wins!\n";
            break;
        case GameState::DRAW:
            std::cout << "🤝 It's a draw!\n";
            break;
        default:
            std::cout << "Game in progress. Next: "
                      << game.getCurrentPlayer()->getName() << "\n";
    }
}

// ─────────────────────────────────────────────
// Main – demo / test scenarios
// ─────────────────────────────────────────────

int main() {
    // ── Scenario 1: Horizontal win ────────────
    std::cout << "=== Scenario 1: Horizontal Win ===\n\n";
    {
        Player alice("Alice (R)", DiscColor::RED);
        Player bob  ("Bob   (B)", DiscColor::BLUE);
        Game   game(&alice, &bob);

        // Alice drops cols 0-3 consecutively; Bob always goes col 6
        std::vector<std::pair<Player*, int>> moves = {
            {&alice, 0}, {&bob, 6},
            {&alice, 1}, {&bob, 6},
            {&alice, 2}, {&bob, 6},
            {&alice, 3},            // Alice wins horizontally on row 5
        };

        for (auto& [p, col] : moves) {
            std::cout << p->getName() << " drops in column " << (col + 1) << "\n";
            game.makeMove(p, col);
            game.getBoard()->display();
            if (game.getGameState() != GameState::IN_PROGRESS) break;
        }
        printResult(game);
    }

    // ── Scenario 2: Vertical win ──────────────
    std::cout << "\n=== Scenario 2: Vertical Win ===\n\n";
    {
        Player alice("Alice (R)", DiscColor::RED);
        Player bob  ("Bob   (B)", DiscColor::BLUE);
        Game   game(&alice, &bob);

        std::vector<std::pair<Player*, int>> moves = {
            {&alice, 3}, {&bob, 4},
            {&alice, 3}, {&bob, 4},
            {&alice, 3}, {&bob, 4},
            {&alice, 3},            // Alice wins vertically in col 3
        };

        for (auto& [p, col] : moves) {
            std::cout << p->getName() << " drops in column " << (col + 1) << "\n";
            game.makeMove(p, col);
            game.getBoard()->display();
            if (game.getGameState() != GameState::IN_PROGRESS) break;
        }
        printResult(game);
    }

    // ── Scenario 3: Diagonal win (\) ──────────
    std::cout << "\n=== Scenario 3: Diagonal Win (\\) ===\n\n";
    {
        Player alice("Alice (R)", DiscColor::RED);
        Player bob  ("Bob   (B)", DiscColor::BLUE);
        Game   game(&alice, &bob);

        // Build bottom-left to top-right diagonal for Alice
        // Positions (row,col): (5,0),(4,1),(3,2),(2,3) — achieved by staggered drops
        std::vector<std::pair<Player*, int>> moves = {
            {&alice, 0},                        // (5,0)
            {&bob,   1}, {&alice, 1},           // (5,1) then (4,1)
            {&bob,   2}, {&bob,   2}, {&alice, 2},  // fill col 2 to row 3
            {&bob,   3}, {&bob,   3}, {&bob,   3}, {&alice, 3}, // fill col 3 to row 2
        };

        for (auto& [p, col] : moves) {
            std::cout << p->getName() << " drops in column " << (col + 1) << "\n";
            game.makeMove(p, col);
            game.getBoard()->display();
            if (game.getGameState() != GameState::IN_PROGRESS) break;
        }
        printResult(game);
    }

    // ── Scenario 4: Invalid move tests ────────
    std::cout << "\n=== Scenario 4: Invalid Moves ===\n\n";
    {
        Player alice("Alice (R)", DiscColor::RED);
        Player bob  ("Bob   (B)", DiscColor::BLUE);
        Game   game(&alice, &bob);

        // Fill column 0 completely (6 discs, alternating)
        for (int i = 0; i < 6; i++) {
            Player* p = (i % 2 == 0) ? &alice : &bob;
            game.makeMove(p, 0);
        }
        game.getBoard()->display();

        std::cout << "Trying to drop into full column 1 (index 0):\n";
        game.makeMove(&alice, 0);   // should fail — column full

        std::cout << "Trying wrong player turn (Bob plays out of turn):\n";
        game.makeMove(&bob, 1);     // should fail — Alice's turn
    }

    // ── Scenario 5: Move after game over ──────
    std::cout << "\n=== Scenario 5: Move After Game Over ===\n\n";
    {
        Player alice("Alice (R)", DiscColor::RED);
        Player bob  ("Bob   (B)", DiscColor::BLUE);
        Game   game(&alice, &bob);

        std::vector<std::pair<Player*, int>> moves = {
            {&alice, 0}, {&bob, 6},
            {&alice, 1}, {&bob, 6},
            {&alice, 2}, {&bob, 6},
            {&alice, 3},            // Alice wins
        };
        for (auto& [p, col] : moves) game.makeMove(p, col);
        printResult(game);

        std::cout << "Trying to play after the game is over:\n";
        game.makeMove(&bob, 5);    // should fail — game over
    }

    return 0;
}
