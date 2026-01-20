#include<iostream>
using namespace std;

enum class PieceType{X,O,EMPTY};

class Player{  
private: 
    string name;
    PieceType piece;
public:
    Player(string name,PieceType p){
        this->name = name;
        piece = p;
    }

    string getName(){
        return name;
    }

    PieceType getPiece(){
        return piece;
    }
};

class Board{
private:
    int size;
    vector<vector<PieceType>>grid;
public:
    Board(int size){
        this->size = size;
        grid.resize(size,vector<PieceType>(size,PieceType::EMPTY));
    }

    int getSize(){
        return this->size;
    }

    PieceType getPiece(int row,int col){
        return grid[row][col];
    }

    void setPiece(int row,int col,PieceType piece){
        this->grid[row][col] = piece;
    }

    bool isCellEmpty(int row,int col){
        return grid[row][col] == PieceType :: EMPTY;
    }

    void printBoard(){
        int n = size;
        for(int i = 0 ; i<n ; i++){
            for(int j = 0 ; j<n ; j++){
                char symbol = '-';
                if(grid[i][j] == PieceType::O){
                    symbol = 'O';
                }
                else if(grid[i][j] == PieceType::X){
                    symbol = 'X';
                }
                cout << symbol << " ";
            }
            cout << endl;
        }
    }
};

class WinningStrategy{
public:
    virtual bool checkWinner(Board *board,int row,int col,PieceType piece) = 0;
};

class StandardWinningStrategy : public WinningStrategy{
public:
    bool checkWinner(Board *board,int row,int col,PieceType piece) override{
        int n = board->getSize();


        bool win = true;
        for(int i = 0 ; i<n ; i++){
            if(board->getPiece(row,i) != piece)win = false;
        }
        if(win)return true;
        
        win = true;
        for(int i = 0 ; i<n ; i++){
            if(board->getPiece(i,col) != piece)win = false;
        }
        if(win)return true;

        
        if(row == col){
            win = true;
            for(int i = 0 ; i<n ; i++){
                if(board->getPiece(i,i) != piece)win = false;
            }
            if(win)return true;
        }
        

        if(row + col == n-1){;
            win = true;
            for(int i = 0 ; i<n ; i++){
                if(board->getPiece(i,n-i-1) != piece)win = false;
            }
            if(win)return true;
        }

        return false;
    }
};

class Game{
private:
    Board* board;
    WinningStrategy* strategy;
    deque<Player*> players;
public:
    Game(int size,vector<Player*>players){
        this->board = new Board(size);
        this->strategy = new StandardWinningStrategy();

        for(auto &player : players){
            this->players.push_back(player);
        }
    }
    void play(){
        int totalMoves = 0;
        int n = board->getSize();
        int maxMoves = n * n;

        while(totalMoves < maxMoves){
            board->printBoard();
            Player* currentPlayer = players.front();

            int r, c;
            cout << currentPlayer->getName() << " (" << (currentPlayer->getPiece() == PieceType::X ? "X" : "O") 
                 << "), enter row and col: ";
            cin >> r >> c;

            if (r < 0 || r >= board->getSize() || c < 0 || c >= board->getSize() || !board->isCellEmpty(r, c)) {
                cout << "Invalid move, try again." << endl;
                continue;
            }

            board->setPiece(r, c, currentPlayer->getPiece());

            if (strategy->checkWinner(board, r, c, currentPlayer->getPiece())) {
                board->printBoard();
                cout << "Congratulations! " << currentPlayer->getName() << " wins!" << endl;
                return;
            }

            players.pop_front();
            players.push_back(currentPlayer);
            totalMoves++;
        }
        cout << "The game is a draw!" << endl;
    }
};

int main() {
    Player* p1 = new Player("Alice", PieceType::X);
    Player* p2 = new Player("Bob", PieceType::O);

    Game* game = new Game(3, {p1, p2});
    game->play();

    return 0;
}