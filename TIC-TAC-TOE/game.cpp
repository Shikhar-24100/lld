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
        vector<vector<Symbol>> grid;
        int size;
        Symbol* emptyCell;
    
    public:
        Board(int s) {
            size = s;
            emptyCell = new Symbol('-');
            grid.resize(size, vector<Symbol>(size, *emptyCell));
        }

        bool isCellEmpty(int row, int col) {
            if(row < 0 || row > size || col < 0 || col >= size) return false;
            return grid[row][col].getMark() == emptyCell->getMark();
        }


        bool placeMark(int row, int col, Symbol* mark){
            if(row < 0 || row >= size || col < 0 || col >= size) return false;

            if(!isCellEmpty(row, col)) return false;

            grid[row][col] = *mark;
            return true;
        }

};


