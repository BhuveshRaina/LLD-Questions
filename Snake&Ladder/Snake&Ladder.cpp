#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <ctime>

using namespace std;

// --- 1. Jumper Interface (Strategy for Cell Effects) ---
class IJumper {
public:
    virtual int getEnd() = 0;
    virtual ~IJumper() {}
};

class Snake : public IJumper {
    int start, end;
public:
    Snake(int start, int end) {
        this->start = start;
        this->end = end;
    }
    int getEnd() override { return this->end; }
};

class Ladder : public IJumper {
    int start, end;
public:
    Ladder(int start, int end) {
        this->start = start;
        this->end = end;
    }
    int getEnd() override { return this->end; }
};

// --- 2. Cell Class (SRP: Manages single cell state) ---
class Cell {
    IJumper* jumper;
public:
    Cell() { this->jumper = nullptr; }
    void setJumper(IJumper* j) { this->jumper = j; }
    
    int applyJumper(int currentPos) {
        if (this->jumper != nullptr) {
            return this->jumper->getEnd();
        }
        return currentPos;
    }
};

// --- 3. Board Class (SRP: Manages the grid) ---
class Board {
    int size;
    vector<Cell*> cells;
public:
    Board(int size) {
        this->size = size;
        for (int i = 0; i <= size; i++) {
            this->cells.push_back(new Cell());
        }
    }

    void addJumper(int start, IJumper* jumper) {
        this->cells[start]->setJumper(jumper);
    }

    int resolvePosition(int pos) {
        if (pos > size) return pos;
        return cells[pos]->applyJumper(pos);
    }

    int getSize() { return this->size; }
};

// --- 4. Dice Strategy (OCP: Easy to add "Crooked Dice") ---
class IDice {
public:
    virtual int roll() = 0;
    virtual ~IDice() {}
};

class StandardDice : public IDice {
    int count;
public:
    StandardDice(int count) { this->count = count; }
    int roll() override {
        int val = 0;
        for(int i=0; i<count; i++) val += (rand() % 6) + 1;
        return val;
    }
};

// --- 5. Player Class ---
class Player {
    string name;
    int position;
public:
    Player(string name) {
        this->name = name;
        this->position = 0;
    }
    string getName() { return this->name; }
    int getPosition() { return this->position; }
    void setPosition(int p) { this->position = p; }
};

// --- 6. Game Controller (High-level Orchestrator) ---
class SnakeLadderGame {
    Board* board;
    IDice* dice;
    deque<Player*> players;

public:
    // Dependency Injection: Pass the board and dice from outside
    SnakeLadderGame(Board* board, IDice* dice, vector<Player*> playersList) {
        this->board = board;
        this->dice = dice;
        for (auto p : playersList) {
            this->players.push_back(p);
        }
    }

    void start() {
        while (true) {
            Player* p = players.front();
            players.pop_front();

            int roll = dice->roll();
            int nextPos = p->getPosition() + roll;

            if (nextPos <= board->getSize()) {
                int finalPos = board->resolvePosition(nextPos);
                
                cout << p->getName() << " rolled a " << roll 
                     << " (" << p->getPosition() << " -> " << finalPos << ")";
                
                if (finalPos != nextPos) {
                    string type = (finalPos > nextPos) ? " [Ladder!]" : " [Snake!]";
                    cout << type;
                }
                cout << endl;

                p->setPosition(finalPos);

                if (finalPos == board->getSize()) {
                    cout << "Winner: " << p->getName() << "!" << endl;
                    return;
                }
            } else {
                cout << p->getName() << " rolled " << roll << " but exceeded board size. Skipping." << endl;
            }

            players.push_back(p);
        }
    }
};

// --- Main: Setup and Execution ---
int main() {
    srand(time(0));

    // 1. Configure Board (Dependency Injection setup)
    Board* board = new Board(100);
    board->addJumper(10, new Snake(10, 2));
    board->addJumper(99, new Snake(99, 10));
    board->addJumper(5, new Ladder(5, 30));
    board->addJumper(40, new Ladder(40, 70));

    // 2. Configure Dice and Players
    IDice* dice = new StandardDice(1);
    Player* p1 = new Player("Alice");
    Player* p2 = new Player("Bob");

    // 3. Initialize and Start Game
    SnakeLadderGame game(board, dice, {p1, p2});
    game.start();

    return 0;
}