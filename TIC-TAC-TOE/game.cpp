#include <iostream>
#include <vector>
#include <queue>
#include <deque>

using namespace std;

//observer patter-for future notification service
class IObserver {
    public:
        virtual void update(string msg) = 0;
        virtual ~IObserver() {}
};

//Symbol/mark - class
class Symbol {
    private:
        char mark;
    
        public:
        Symbol(char m){
            mark = m;
        }

        char getMark() {
            return mark;
        }
};

//Board class - manages the grids
class Board {
    private:
        vector<vector<Symbol*>> grid;
        int size;
        Symbol* emptyCell;
    
    public:
        Board(int s) {
            size = s;
            emptyCell = new Symbol('-');
            grid.resize(size, vector<Symbol*>(size, emptyCell));
        }

        bool isCellEmpty(int row, int col) {
            if(row < 0 || row >= size || col < 0 || col >= size) return false;
            return grid[row][col]->getMark() == emptyCell->getMark();
        }


        bool placeMark(int row, int col, Symbol* mark){
            if(row < 0 || row >= size || col < 0 || col >= size) return false;

            if(!isCellEmpty(row, col)) return false;

            grid[row][col] = mark;
            return true;
        }


        Symbol* getCell(int row, int col){
            if(row < 0 || row >= size || col < 0 || col >= size) return emptyCell;
            return grid[row][col];
        }

        int getSize() const {
            return size;
        }

        void display() {
            for(int i = 0; i < size; i++){
                for(int j = 0; j < size; j++){
                    cout << grid[i][j]->getMark() << " ";
                }
                cout << endl;
            }
        }

};



//player class
class TicTacToePlayer {
    private:
        int playerId;
        string name;
        Symbol* symbol;
        int score;
    public:
        TicTacToePlayer(int playerId, string n, Symbol* s) : playerId(playerId), name(n), symbol(s), score(0) {}

        //getters
        string getName() const {return name;}

        Symbol* getSymbol() const {return symbol;}

        int getScore() const {return score;}

        void incrementScore() {score++;}

        ~TicTacToePlayer() {
            delete symbol;
        }
};


//strategy pattern for game rules
class TicTacToeRules {
    public:
        virtual bool isValidMove(Board* board, int row, int col) = 0;
        virtual bool checkWin(Board* board, Symbol* symbol) = 0;    
        virtual bool checkDraw(Board* board) = 0;
        virtual ~TicTacToeRules() {}
};


class StandardTicTacToeRules : public TicTacToeRules {
    public:
        bool isValidMove(Board* board, int row, int col) override {
            return board->isCellEmpty(row, col);
        }

        bool checkWin(Board* board, Symbol* symbol) override {
            int size = board->getSize();

            //checkrows
            for(int i = 0; i < size; i++){
                bool rowWin = true;
                for(int j = 0; j < size; j++){
                    if(board->getCell(i, j) != symbol){
                        rowWin = false;
                        break;
                    }
                }
                if(rowWin) return true;
            }


            //check columns
            for(int j = 0; j < size; j++){
                bool colWin = true;
                for(int i = 0; i < size; i++){
                    if(board->getCell(i, j) != symbol){
                        colWin = false;
                        break;
                    }
                }
                if(colWin) return true;
            }

            //check diagonals
            bool diag1Win = true;
            for(int i = 0; i < size; i++){
                if(board->getCell(i, i) != symbol){
                    diag1Win = false;
                    break;
                }
            }

            bool diag2Win = true;
            for(int i = 0; i < size; i++){
                if(board->getCell(i, size - 1 - i) != symbol){
                    diag2Win = false;
                    break;
                }
            }

            return diag1Win || diag2Win;
        }


        bool checkDraw(Board* board) override {
            int size = board->getSize();
            for(int i = 0; i < size; i++){
                for(int j = 0; j < size; j++){
                    if(board->isCellEmpty(i, j)) return false;
                }
            }
            return true;
        }
};



///game class
class TicTacToeGame {
    private:
        Board* board;
        deque<TicTacToePlayer*> players;
        TicTacToeRules* rules;
        vector<IObserver*> observers;
        bool gameOver;
    
    public:
        TicTacToeGame(int boardSize) {
            board = new Board(boardSize);
            rules = new StandardTicTacToeRules();
            gameOver = false;
        }

        void addPlayer(TicTacToePlayer* player) {
            players.push_back(player);
        }

        void addObserver(IObserver* observer) {
            observers.push_back(observer);
        }

        void notify(string msg) {
            for(auto observer : observers) {
                observer->update(msg);
            }
        }

        void setRule(TicTacToeRules* rule) {
            this->rules = rule;
        }


        void play() {
            if(players.size() < 2) {
                cout << "Not enough players to start the game." << endl;
                return;
            }

            notify("Game started with " + to_string(players.size()) + " players.");

            while(!gameOver) {
                board->display();

                //take out the current player from deque
                TicTacToePlayer* currentPlayer = players.front();
                
                cout << currentPlayer->getName() << " ("<< currentPlayer->getSymbol()->getMark() << ")'s turn." << endl;

                int row, col;
                cin >> row >> col;

                //check if move is valid
                if(rules->isValidMove(board, row, col)) {
                    board->placeMark(row, col, currentPlayer->getSymbol());
                    notify(currentPlayer->getName() + " placed " + currentPlayer->getSymbol()->getMark() + " at (" + to_string(row) + ", " + to_string(col) + ").");

                    if(rules->checkWin(board, currentPlayer->getSymbol())) {
                        board->display();
                        cout << currentPlayer->getName() << " wins!" << endl;
                        currentPlayer->incrementScore();

                        notify(currentPlayer->getName() + " wins the game!");

                        gameOver = true;
                    }

                    else if(rules->checkDraw(board)) {
                        board->display();
                        cout << "Game is a draw!" << endl;
                        notify("Game ended in a draw.");
                        gameOver = true;
                    }
                    else {
                        players.pop_front();
                        players.push_back(currentPlayer); //add back to the end of deque
                    }
                }

                else{
                    cout << "Invalid move. Try again." << endl;
                    notify(currentPlayer->getName() + " attempted an invalid move at (" + to_string(row) + ", " + to_string(col) + ").");
                }
            }
        }

        ~TicTacToeGame() {
            delete board;
            delete rules;
            for(auto player : players) {
                delete player;
            }
        }
    
};


enum GameType {
    STANDARD,
    CUSTOM
};

class TicTacToeGameFactory {
    public:
        static TicTacToeGame* createGame(GameType type, int boardSize) {
            if(type == GameType::STANDARD) {
                return new TicTacToeGame(boardSize);
            } else {
                // For custom game, we can add more logic here
                return new TicTacToeGame(boardSize);
            }
            return nullptr;
        }
};


//simple observer implementation
class ConsoleNotifier : public IObserver {
    public:
        void update(string msg) override {
            cout << "[Notification] " << msg << endl;
        }
};
