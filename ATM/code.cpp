#include <iostream>
#include <string>

using namespace std;

class ATM;
class Card;

// --- 1. State Interface ---
class ATMState {
public:
    virtual void insertCard(ATM* atm, Card* card) = 0;
    virtual void authenticatePin(ATM* atm, int pin) = 0;
    virtual void withdrawCash(ATM* atm, int amount) = 0;
    virtual void ejectCard(ATM* atm) = 0;
    virtual string getStateName() = 0;
    virtual ~ATMState() {}
};

// --- 2. Card Class ---
class Card {
private:
    int pin;
    int balance;
public:
    Card(int p, int b) {
        this->pin = p;
        this->balance = b;
    }
    bool checkPin(int p) { return this->pin == p; }
    int getBalance() { return this->balance; }
    void deduct(int amt) { this->balance -= amt; }
};

// --- 3. ATM Context ---
class ATM {
private:
    ATMState *idleState, *hasCardState, *pinEnteredState;
    ATMState* currentState;
    Card* currentCard;
    int atmBalance;

public:
    ATM(int initialCash);
    
    // State Getters
    ATMState* getIdleState() { return idleState; }
    ATMState* getHasCardState() { return hasCardState; }
    ATMState* getPinEnteredState() { return pinEnteredState; }

    // Setters/Getters
    void setState(ATMState* state) { this->currentState = state; }
    void setCard(Card* card) { this->currentCard = card; }
    Card* getCard() { return this->currentCard; }
    int getAtmBalance() { return atmBalance; }
    void deductAtm(int amt) { atmBalance -= amt; }
    string getCurrentStateName() { return currentState->getStateName(); }

    // Standard Operations
    void insertCard(Card* c) { currentState->insertCard(this, c); }
    void enterPin(int p) { currentState->authenticatePin(this, p); }
    void withdraw(int a) { currentState->withdrawCash(this, a); }
    void eject() { currentState->ejectCard(this); }

    // --- The Simulation Function (The "Facade" Method) ---
    void processTransaction(Card* card) {
        cout << "\n--- Welcome to the ATM ---\n";
        this->insertCard(card);

        while (getCurrentStateName() != "IdleState") {
            if (getCurrentStateName() == "HasCardState") {
                int inputPin;
                cout << "Enter PIN: ";
                cin >> inputPin;
                this->enterPin(inputPin);
            } 
            else if (getCurrentStateName() == "PinEnteredState") {
                cout << "\n1. Withdraw Cash\n2. Eject Card\nChoice: ";
                int choice;
                cin >> choice;

                if (choice == 1) {
                    int amount;
                    cout << "Enter amount: ";
                    cin >> amount;
                    this->withdraw(amount);
                } else {
                    this->eject();
                }
            }
        }
        cout << "--- Session Ended ---\n";
    }
};

// --- 4. Concrete States Implementation ---

class IdleState : public ATMState {
public:
    string getStateName() override { return "IdleState"; }
    void insertCard(ATM* atm, Card* card) override;
    void authenticatePin(ATM* atm, int pin) override { cout << "Error: Insert card first.\n"; }
    void withdrawCash(ATM* atm, int amount) override { cout << "Error: Insert card first.\n"; }
    void ejectCard(ATM* atm) override { cout << "Error: No card inside.\n"; }
};

class HasCardState : public ATMState {
public:
    string getStateName() override { return "HasCardState"; }
    void insertCard(ATM* atm, Card* card) override { cout << "Error: Card already in.\n"; }
    void authenticatePin(ATM* atm, int pin) override;
    void withdrawCash(ATM* atm, int amount) override { cout << "Error: Enter PIN first.\n"; }
    void ejectCard(ATM* atm) override;
};

class PinEnteredState : public ATMState {
public:
    string getStateName() override { return "PinEnteredState"; }
    void insertCard(ATM* atm, Card* card) override { cout << "Error: Card already in.\n"; }
    void authenticatePin(ATM* atm, int pin) override { cout << "Error: Already verified.\n"; }
    void withdrawCash(ATM* atm, int amount) override;
    void ejectCard(ATM* atm) override;
};

// --- 5. Logic & Transitions ---

ATM::ATM(int initialCash) {
    this->atmBalance = initialCash;
    this->currentCard = nullptr;
    this->idleState = new IdleState();
    this->hasCardState = new HasCardState();
    this->pinEnteredState = new PinEnteredState();
    this->currentState = idleState;
}

void IdleState::insertCard(ATM* atm, Card* card) {
    cout << "System: Card detected.\n";
    atm->setCard(card);
    atm->setState(atm->getHasCardState());
}

void HasCardState::authenticatePin(ATM* atm, int pin) {
    if (atm->getCard()->checkPin(pin)) {
        cout << "System: PIN Correct.\n";
        atm->setState(atm->getPinEnteredState());
    } else {
        cout << "System: Incorrect PIN. Please try again.\n";
    }
}

void HasCardState::ejectCard(ATM* atm) {
    cout << "System: Returning card...\n";
    atm->setCard(nullptr);
    atm->setState(atm->getIdleState());
}

void PinEnteredState::withdrawCash(ATM* atm, int amount) {
    Card* c = atm->getCard();
    if (amount <= c->getBalance() && amount <= atm->getAtmBalance()) {
        c->deduct(amount);
        atm->deductAtm(amount);
        cout << "System: Dispensed " << amount << ". Card Balance: " << c->getBalance() << endl;
    } else {
        cout << "System: Transaction Failed (Insufficient Funds/ATM empty).\n";
    }
}

void PinEnteredState::ejectCard(ATM* atm) {
    cout << "System: Returning card. Thank you for using our ATM!\n";
    atm->setCard(nullptr);
    atm->setState(atm->getIdleState());
}

// --- Main Simulation ---
int main() {
    ATM myATM(20000); // ATM with 20k cash
    Card myCard(1234, 5000); // Card with PIN 1234 and 5k balance

    myATM.processTransaction(&myCard);

    return 0;
}