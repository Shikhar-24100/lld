#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
using namespace std;
//enums
enum class NotificationChannelType {
    EMAIL,
    SMS,
    IN_APP,
    PUSH
};

enum class MessageType {
    DIRECT,
    GROUP
};


//user entities
class User {
private:
    string id;
    string name;
    string email;
    string phoneNumber;
    vector<NotificationChannelType> preferredChannels;
public:
    User(string id, string name, string email, string phoneNumber, vector<NotificationChannelType> channels)
        : id(id), name(name), email(email), phoneNumber(phoneNumber), preferredChannels(channels) {}

    //getters
    string getId() const {return id;}
    string getName() const {return name;}
    string getEmail() const {return email;}
    string getPhoneNumber() const {return phoneNumber;}
    vector<NotificationChannelType> getPreferredChannels() const {return preferredChannels;}
};




