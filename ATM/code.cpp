#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

// --- Forward Declarations ---
class ATM;

// --- 1. Card Class (Encapsulates User Data) ---
class Card {
private:
    int pin;
    int accountBalance;
    int withdrawalLimit;
public:
    Card(int pin, int balance, int limit) {
        this->pin = pin;
        this->accountBalance = balance;
        this->withdrawalLimit = limit;
    }

    bool validatePin(int inputPin) {
        return this->pin == inputPin;
    }

    int getAccountBalance() { return this->accountBalance; }
    int getWithdrawalLimit() { return this->withdrawalLimit; }

    void deductBalance(int amount) {
        this->accountBalance -= amount;
    }
};

// --- 2. State Interface ---
class ATMState {
public:
    virtual void insertCard(ATM* atm, Card* card) = 0;
    virtual void authenticatePin(ATM* atm, int pin) = 0;
    virtual void withdrawCash(ATM* atm, int amount) = 0;
    virtual void ejectCard(ATM* atm) = 0;
    virtual ~ATMState() {}
};

// --- 3. ATM Context ---
class ATM {
private:
    ATMState* currentState;
    Card* currentCard;
    int atmCash;

public:
    ATM(int atmCash) {
        this->atmCash = atmCash;
        this->currentCard = nullptr;
        this->currentState = nullptr; 
    }

    void setState(ATMState* state) { this->currentState = state; }
    
    void setCard(Card* card) { this->currentCard = card; }
    Card* getCard() { return this->currentCard; }

    int getAtmCash() { return this->atmCash; }
    void deductAtmCash(int amount) { this->atmCash -= amount; }

    // Logic Delegations
    void insertCard(Card* card) { currentState->insertCard(this, card); }
    void enterPin(int pin) { currentState->authenticatePin(this, pin); }
    void withdraw(int amount) { currentState->withdrawCash(this, amount); }
    void ejectCard() { currentState->ejectCard(this); }
};

// --- 4. Concrete States ---

class IdleState : public ATMState {
public:
    void insertCard(ATM* atm, Card* card) override;
    void authenticatePin(ATM* atm, int pin) override { cout << "Insert card first.\n"; }
    void withdrawCash(ATM* atm, int amount) override { cout << "Insert card first.\n"; }
    void ejectCard(ATM* atm) override { cout << "No card inside.\n"; }
};

class HasCardState : public ATMState {
public:
    void insertCard(ATM* atm, Card* card) override { cout << "Card already in.\n"; }
    void authenticatePin(ATM* atm, int pin) override;
    void withdrawCash(ATM* atm, int amount) override { cout << "Enter PIN first.\n"; }
    void ejectCard(ATM* atm) override;
};

class PinEnteredState : public ATMState {
public:
    void insertCard(ATM* atm, Card* card) override { cout << "Card already in.\n"; }
    void authenticatePin(ATM* atm, int pin) override { cout << "PIN already verified.\n"; }
    void withdrawCash(ATM* atm, int amount) override;
    void ejectCard(ATM* atm) override;
};

// --- 5. State Transition Implementations ---

void IdleState::insertCard(ATM* atm, Card* card) {
    cout << "[Idle] Card accepted.\n";
    atm->setCard(card);
    atm->setState(new HasCardState());
}

void HasCardState::authenticatePin(ATM* atm, int pin) {
    if (atm->getCard()->validatePin(pin)) {
        cout << "[HasCard] PIN Correct.\n";
        atm->setState(new PinEnteredState());
    } else {
        cout << "[HasCard] Incorrect PIN! Card blocked in real life, but try again here.\n";
    }
}

void HasCardState::ejectCard(ATM* atm) {
    cout << "[HasCard] Card ejected.\n";
    atm->setCard(nullptr);
    atm->setState(new IdleState());
}

void PinEnteredState::withdrawCash(ATM* atm, int amount) {
    Card* card = atm->getCard();

    // Mult-level validation (The "Standard" Logic)
    if (amount > card->getAccountBalance()) {
        cout << "[Error] Insufficient Account Balance!\n";
    } else if (amount > card->getWithdrawalLimit()) {
        cout << "[Error] Amount exceeds Card Daily Limit!\n";
    } else if (amount > atm->getAtmCash()) {
        cout << "[Error] ATM does not have enough physical cash!\n";
    } else {
        // Success
        card->deductBalance(amount);
        atm->deductAtmCash(amount);
        cout << "[Success] Dispensing " << amount << ".\n";
        cout << "Remaining Account Bal: " << card->getAccountBalance() << endl;
        
        // Auto-eject after transaction
        this->ejectCard(atm);
    }
}

void PinEnteredState::ejectCard(ATM* atm) {
    cout << "[PinEntered] Card ejected. Thank you!\n";
    atm->setCard(nullptr);
    atm->setState(new IdleState());
}

// --- 6. Main Scenario ---
int main() {
    ATM* atm = new ATM(10000); // ATM has 10,000 cash
    atm->setState(new IdleState());

    // User Card: PIN=1234, Balance=5000, Daily Limit=2000
    Card* userCard = new Card(1234, 5000, 2000);

    cout << "--- Attempt 1: Too much money ---\n";
    atm->insertCard(userCard);
    atm->enterPin(1234);
    atm->withdraw(3000); // Fails (Limit is 2000)

    cout << "\n--- Attempt 2: Successful ---\n";
    atm->insertCard(userCard);
    atm->enterPin(1234);
    atm->withdraw(1500); // Succeeds

    return 0;
}