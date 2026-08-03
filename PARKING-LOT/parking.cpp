#include<iostream>
#include<string>
#include<vector>
#include<unordered_map>
#include<ctime>
#include<cmath>
#include<algorithm>
using namespace std;

// #1 ENUMS
enum class VehicleType {SMALL = 1, MEDIUM = 2, LARGE = 3};
enum class SpotType    {SMALL = 1, MEDIUM = 2, LARGE = 3};
enum class SpotStatus  {FREE, OCCUPIED};


// #2 Core Domain Classes

class Vehicle {
    private:
        string licensePlate;
        VehicleType type;
    
    public:
        //constuctor
        Vehicle(string plate, VehicleType vehilcletype) : licensePlate(plate), type(vehilcletype) {}

        string getLicensePlate() const {return licensePlate;}
        VehicleType getType() const {return type;}
        
};


class ParkingSpot {
    private:
        string spotId;
        int floorId;
        SpotType type;
        SpotStatus status;
        Vehicle* parkedVehicle;

    public:
        ParkingSpot(string id, int floor, SpotType spotType)
            : spotId(id), floorId(floor), type(spotType), status(SpotStatus::FREE), parkedVehicle(nullptr) {}

        bool canFit(VehicleType vehicleType) const {
            return (status == SpotStatus::FREE) &&
                   (static_cast<int>(type) >= static_cast<int>(vehicleType));
        }

        bool parkVehicle(Vehicle* vehicle){
            if(!canFit(vehicle->getType())) return false;
            parkedVehicle = vehicle;
            status = SpotStatus::OCCUPIED;
            return true;
        }

        void removeVehicle() {
            parkedVehicle = nullptr;
            status = SpotStatus::FREE;
        }


        // bool i
        // getters
        string getSpotId() const {return spotId;}
        Vehicle* getParkedVehicle() const {return parkedVehicle;}
};


class Ticket {
    private:
        string ticketId;
        time_t entryTime;
        string licensePlate;
        ParkingSpot* assignedSpot;

    public:
        Ticket(string id, string plate, ParkingSpot* spot)
            :ticketId(id), licensePlate(plate), assignedSpot(spot) {
                entryTime = time(nullptr);
            }
        
        //getters

        string getTicketId() const {return ticketId;}
        ParkingSpot* getAssignedSpot() const {return assignedSpot;}
        time_t getEntryTime() const {return entryTime;}
};



// 3. Structural Classes


class ParkingFloor {

    private:
        int floorId;
        vector<ParkingSpot*> spots;

    public:
        ParkingFloor(int id) : floorId(id) {}

        void addSpot(ParkingSpot* spot){
            spots.push_back(spot);
        }

        ParkingSpot* findAvailableSpot(VehicleType type){
            for(auto spot: spots){
                if(spot->canFit(type)){
                    return spot;
                }
            }
            return nullptr;
        }

        //getters

};



class ParkingLot {
    private:
        static ParkingLot* instance;
        string name;
        vector<ParkingFloor*> floors;
        unordered_map<string, Ticket*> activeTickets;
        int ticketCounter;

    public:
        ParkingLot(string lotName) : name(lotName), ticketCounter(1) {}
        static ParkingLot* getInstance(string lotName = "City Central Parking") {
            if(instance == nullptr) {
                instance = new ParkingLot(lotName);
            }
            return instance;
        }

        // void addFloor(){
        //     // 
        // }

        ParkingSpot* findAvailableSpot(VehicleType type) {
            for(auto floor: floors) {
                ParkingSpot* spot = floor->findAvailableSpot(type);
                if(spot!=nullptr) return spot;
            }
            return nullptr;
        }

        void registerTicket(Ticket* ticket){
            activeTickets[ticket->getTicketId()] = ticket;
        }

        void removeTicket(const string& ticketId){
            Ticket* ticket = activeTickets[ticketId];
            activeTickets.erase(ticketId);
            delete ticket;
        }

        string generateTicketId() {
            return "TICK-" + to_string(ticketCounter++);
        }
};




class Gate {
    protected:
        string gateId;
    public:
         Gate(string id) : gateId(id) {}
         string getGateId() const {return gateId;}
};


class EntryGate : public Gate {
    public:
        EntryGate(string id): Gate(id) {}

        Ticket* processEntry(Vehicle* vehicle){
            ParkingLot* lot = ParkingLot::getInstance();
            ParkingSpot* spot = lot->findAvailableSpot(vehicle->getType());

            if(spot == nullptr) {
                std::cout << "[Entry Gate " << gateId << "] No available spots for " 
                      << vehicle->getLicensePlate() << "!\n";
            return nullptr;
            }

            spot->parkVehicle(vehicle);

            string ticketId = lot->generateTicketId();
            Ticket* ticket = new Ticket(ticketId, vehicle->getLicensePlate(), spot);
            lot->registerTicket(ticket);

            std::cout << "[Entry Gate " << gateId << "] Ticket " << ticketId 
                  << " issued for " << vehicle->getLicensePlate() 
                  << " at Spot " << spot->getSpotId() << "\n";

            return ticket;
        }
};


class ExitGate: public Gate {
    private:
        double calculateFee(time_t entryTime, time_t exitTime, VehicleType type) const {
            double durationInSeconds = difftime(exitTime, entryTime);

            double hours = ceil(durationInSeconds / 3600.0);
            if(hours < 1.0) hours = 1.0;

            double hourlyRate = 2.0;
            if(type == VehicleType::MEDIUM) hourlyRate = 5.0;
            else if(type == VehicleType::LARGE) hourlyRate = 10.0;

            return hours*hourlyRate;
        }
    
        public:
            ExitGate(string id): Gate(id) {}

            double processExit(Ticket* ticket){
                if(ticket == nullptr) return 0.0;

                ParkingSpot* spot = ticket->getAssignedSpot();
                Vehicle* vehicle = spot->getParkedVehicle();

                time_t exitTime = time(nullptr);
                double fee = calculateFee(ticket->getEntryTime(), exitTime, vehicle->getType());

                std::cout << "[Exit Gate " << gateId << "] Vehicle " << vehicle->getLicensePlate() 
                  << " exiting. Spot " << spot->getSpotId() 
                  << " freed. Total Fee: $" << fee << "\n";

                spot->removeVehicle();
                ParkingLot::getInstance()->removeTicket(ticket->getTicketId());
                return fee;
            }
}







