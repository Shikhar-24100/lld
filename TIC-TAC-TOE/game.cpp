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
