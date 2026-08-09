#include<iostream>
#include<vector>
#include<queue>
#include<cmath>
using namespace std;
#include <unique_ptr>

enum class Direction { UP, DOWN, IDLE};
enum class ElevatorStatus {IDLE, MOVING, STOPPED_DOORS_OPEN};
enum class RequestType {CAB, HALL};


//Abstract strategy interface
class DispatchStrategy {
    public: 
        virtual ~DispatchStrategy() = default;

        virtual int calculateCost(const Elevator& elevator, int pickup_floor, Direction req_direction, int total_floors) const = 0;

}

//strategy-1: original direction-aware cost algorithm
class DirectionAwareCostStrategy : public DispatchStrategy {
    public:
        int calculateCost(const Elevator& elevator, int pickup_floor, Direction req_direction, int total_floors) const override {
            int current = elevator.getCurrentFloor();
            Direction dir = elevator.getDirection();

            int dist = abs(current - pickup_floor);

            if(dir == Direction::IDLE) return dist + 1;

            if(req_direction == Direction::UP && dir == Direction::UP && pickup_floor >= current){
                return dist;
            }

            if(req_direction == Direction::DOWN && dir == Direction::DOWN && pickup_floor <= current){
                return dist;
            }

            return 2*total_floors + dist; //penalty for changing direction
        }
};



// Strategy 2: Simple Nearest-Elevator First (great for light traffic or testing)
class NearestElevatorStrategy : public DispatchStrategy {
public:
    int calculateCost(const Elevator& elevator, 
                      int pickup_floor, 
                      Direction req_direction, 
                      int total_floors) const override {
        return std::abs(elevator.getCurrentFloor() - pickup_floor);
    }
};







//elevator controller class

class ElevatorController {
    private:
        vector<Elevator> elevators;
        int num_floors;
        unique_ptr<DispatchStrategy> strategy;


        // //dispatcher scoring logic
        // int calculateCost(const Elevator& elevator, int pickup_floor, Direction req_direction){
        //     int current = elevator.getCurrentFloor();
        //     Direction dir = elevator.getDirection();

        //     int dist = abs(current - pickup_floor);

        //     if(dir == Direction::IDLE) return dist + 1;

        //     if(req_direction == Direction::UP && dir == Direction::UP && pickup_floor >= current){
        //         return dist;
        //     }

        //     if(req_direction == Direction::DOWN && dir == Direction::DOWN && pickup_floor <= current){
        //         return dist;
        //     }

        //     return 2*num_floors + dist; //penalty for changing direction 
        // }


        void handleHallRequest(int pickup_floor, Direction direction){
            int best_elevator_id = -1;
            int min_cost = INT_MAX;
            
            for(size_t i = 0; i < elevators.size(); i++){
                int cost = strategy->calculateCost(elevators[i], pickup_floor, direction);
                if(cost < min_cost){
                    min_cost = cost;
                    best_elevator_id = i;
                }
            }

            std::cout << "\n>>> [Controller] Hall Request on Floor " << pickup_floor 
                  << " (" << (direction == Direction::UP ? "UP" : "DOWN") 
                  << ") -> Assigned to Elevator " << best_elevator_id 
                  << " (Cost: " << min_cost << ")\n";

            elevators[best_elevator_id].addHallRequest(pickup_floor, direction);

        }


        void handleCabRequest(int elevator_id, int destination_floor) {
            std::cout << "\n>>> [Controller] Cab Request inside Elevator " << elevator_id 
                  << " -> Target Floor " << destination_floor << "\n";

            elevators[elevator_id].addCabRequest(destination_floor);
        }

    public:
        ElevatorController(int num_elevators, int num_floors) : num_floors(num_floors) {
            for (int i = 0; i < num_elevators; ++i) {
                elevators.emplace_back(i, 0);
            }
        }
        
        void setStrategy(unique_ptr<DispatchStrategy> new_strategy){
            strategy = move(new_strategy);
        }
        // Single unified entry point
        void handleRequest(const Request& req) {
            if (req.floor < 0 || req.floor >= num_floors) {
                std::cout << "[Controller] Invalid floor: " << req.floor << "\n";
                return;
            }

            if (req.type == RequestType::HALL) {
                handleHallRequest(req.floor, req.direction);
            } else if (req.type == RequestType::CAB) {
                if (req.elevator_id < 0 || req.elevator_id >= static_cast<int>(elevators.size())) {
                    std::cout << "[Controller] Invalid elevator ID: " << req.elevator_id << "\n";
                    return;
                }
                handleCabRequest(req.elevator_id, req.floor);
            }
        }

        void step() {
            for (auto& elevator : elevators) {
                elevator.step();
            }
        }

        void printStatus() const {
            std::cout << "\n\n----------------- SYSTEM STATUS -----------------";
            for (const auto& e : elevators) {
                std::cout << "\nElevator " << e.getId() 
                        << " | Floor: " << e.getCurrentFloor() 
                        << " | Dir: " << (e.getDirection() == Direction::UP ? "UP" : 
                                        (e.getDirection() == Direction::DOWN ? "DOWN" : "IDLE"))
                        << " | Pending Requests: " << (e.hasRequests() ? "Yes" : "No");
            }
            std::cout << "\n-------------------------------------------------\n";
        }
        
    
};

class Elevator {
    private:
        int id;
        int current_floor;
        Direction direction;
        ElevatorStatus status;

        //min-heap for UP-requests
        priority_queue<int, vector<int>, greater<int>> up_queue;
        //max-heap for down requests
        priority_queue<int> down_queue;
    
    public:
        Elevator(int id, int start_floor = 0)
            : id(id), current_floor(start_floor), direction(Direction::IDLE), status(ElevatorStatus::IDLE) {}
        
        
        // add destination inside the cab
        void addCabRequest(int destination_floor){
            if(destination_floor == current_floor){
                stopAtFloor();
                // cout << "Elevator " << id << " is already at floor " << current_floor << ". No action taken." << endl;
                return;
            }

            if(destination_floor > current_floor) {
                up_queue.push(destination_floor);
                if(direction == Direction::IDLE) direction = Direction::UP;
            } else{
                down_queue.push(destination_floor);
                if(direction == Direction::IDLE) direction = Direction::DOWN;
            }

            status = ElevatorStatus::MOVING;
        }


        
        void addHallRequest(int pickup_floor, Direction req_direction){
            if(pickup_floor == current_floor){
                stopAtFloor();
                // cout << "Elevator " << id << " is already at floor " << current_floor << ". No action taken." << endl;
                return;
            }

            if(direction == Direction::UP) {
                if(req_direction == Direction:: UP && pickup_floor > current_floor){
                    up_queue.push(pickup_floor);
                }else{
                    down_queue.push(pickup_floor);
                }
            }

            else if(direction == Direction:: DOWN) {
                if(req_direction == Direction::DOWN && pickup_floor < current_floor){
                    down_queue.push(pickup_floor);
                } else{
                    up_queue.push(current_floor);
                }
            }

            else{//idle
                if(req_direction == Direction::UP){
                    up_queue.push(pickup_floor);
                }else{
                    down_queue.push(pickup_floor);
                }
                direction = (pickup_floor > current_floor)?Direction::UP : Direction::DOWN;
                status = ElevatorStatus::MOVING;
            }


            // if(pickup_floor > current_floor && req_direction==Direction::UP) {
            //     up_queue.push(pickup_floor);
            //     if(direction == Direction::IDLE) direction = Direction::UP;
            // } else{
            //     down_queue.push(pickup_floor);
            //     if(direction == Direction::IDLE) direction = Direction::DOWN;
            // }

            status = ElevatorStatus::MOVING;
        }

        void stopAtFloor(){
            status = ElevatorStatus::STOPPED_DOORS_OPEN;
            cout << "Elevator " << id << " stopped at floor " << current_floor << ". Doors are open." << endl;

            if(direction == Direction::UP){
                while(!up_queue.empty() && up_queue.top() == current_floor){
                    up_queue.pop();
                }
            } else if(direction == Direction::DOWN){
                while(!down_queue.empty() && down_queue.top() == current_floor){
                    down_queue.pop();
                }
            }
        }

        //main simulation step for the elevator, called in each time unit
        void step(){

            //theres no request, elevator is idle
            if(direction == Direction::IDLE && up_queue.empty() && down_queue.empty()){
                status = ElevatorStatus::IDLE;
                return;
            }

            //up sweep
            if(direction == Direction::UP){
                if(up_queue.empty()){
                    if(!down_queue.empty()){
                        direction = Direction::DOWN;
                        step();
                    } else{
                        direction = Direction::IDLE;
                        status = ElevatorStatus::IDLE;
                    }
                    return;
                }

                if(current_floor == up_queue.top()){
                    stopAtFloor();
                } else{
                    status = ElevatorStatus::MOVING;
                    current_floor++;
                    cout << "Elevator " << id << " moving up to floor " << current_floor << endl;
                    if(current_floor == up_queue.top()){
                        stopAtFloor();
                    }
                }
            }

            //down sweep
            else if(direction == Direction::DOWN){
                if(down_queue.empty()){
                    if(!up_queue.empty()){
                        direction = Direction::UP;
                        step();
                    } else{
                        direction = Direction::IDLE;
                        status = ElevatorStatus::IDLE;
                    }
                    return;
                }

                if(current_floor == down_queue.top()){
                    stopAtFloor();
                } else{
                    status = ElevatorStatus::MOVING;
                    current_floor--;
                    cout << "Elevator " << id << " moving down to floor " << current_floor << endl;
                    if(current_floor == down_queue.top()){
                        stopAtFloor();
                    }
                }
            }
        }


        //getter
        int getId() const {return id;}
        int getCurrentFloor() const {return current_floor;}
        Direction getDirection() const {return direction;}
        ElevatorStatus getStatus() const {return status;}
        bool hasRequests() const {return !up_queue.empty() || !down_queue.empty();}
};



//request-class(struct) to represents the requests made by the user 
struct Request {
    RequestType type;
    int floor; //pickup floor -> Hall, destination floor -> cab
    Direction direction; //only relevant for hall requests
    int elevator_id; //-1->HALL,id->CAB
    
    
    static Request Hall(int pickup_floor, Direction dir){
        return {RequestType::HALL, pickup_floor, dir, -1};
    }

    static Request Cab(int elevator_id, int destination_floor) {
        return {RequestType::CAB, destination_floor, Direction::IDLE, elevator_id};
    }
};








int main() {
    // 2 Elevators, 10 Floors (0 through 9)
    ElevatorController controller(2, 10);

    controller.printStatus();

    // 1. Hall Calls
    controller.handleRequest(Request::Hall(3, Direction::UP));   // Floor 3 UP
    controller.handleRequest(Request::Hall(7, Direction::DOWN)); // Floor 7 DOWN

    // 2. Step Ticks 1 to 4
    for (int tick = 1; tick <= 4; ++tick) {
        std::cout << "\n--- TICK " << tick << " ---";
        controller.step();
    }

    // 3. Passenger inside Elevator 0 selects Floor 8
    controller.handleRequest(Request::Cab(0, 8));

    // 4. Step Ticks 5 to 10
    for (int tick = 5; tick <= 10; ++tick) {
        std::cout << "\n--- TICK " << tick << " ---";
        controller.step();
    }

    controller.printStatus();
    return 0;
}