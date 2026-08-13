#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include<unordered_map>
#include<algorithm>
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


//message entity
class Message {
private:
    string messageId;
    string senderId;
    string recipientId; //can be a userId or a groupId
    string content;
    MessageType type;
    long long timestamp;
public:
    Message(std::string messageId, std::string senderId, std::string recipientId,
            std::string content, MessageType type)
        : messageId(messageId), senderId(senderId), recipientId(recipientId),
          content(content), type(type) {
        timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count();
    }

    //getters;
    std::string getMessageId() const { return messageId; }
    std::string getSenderId() const { return senderId; }
    std::string getRecipientId() const { return recipientId; }
    std::string getContent() const { return content; }
    MessageType getType() const { return type; }
    long long getTimestamp() const { return timestamp;}
};



//group entity
class Group {
private:
    string groupId;
    string name;
    vector<string> memberUserIds;
public:
    Group(string groupId, string name, vector<string> memberUserIds)
        : groupId(groupId), name(name), memberUserIds(memberUserIds) {}

    //getters
    string getGroupId() const {return groupId;}
    string getName() const {return name;}
    vector<string> getMemberUserIds() const {return memberUserIds;}

    void addMember(string &userId){
        memberUserIds.push_back(userId);
    }
};




//strategy interface
class INotificationStrategy {
public:
    virtual ~INotificationStrategy() = default;
    virtual void sendNotification(const User &recipient, const Message &message) = 0;
};

class EmailNotificationStrategy: public INotificationStrategy {
public:
    void sendNotification(const User& recipient, const Message &message) override {
        cout<<"[EMAIL] Sent to "<<recipient.getEmail()
        << " -> Content: \""<<message.getContent() <<"\"\n";    
    }
};

class SMSNotificationStrategy: public INotificationStrategy {
public:
    void sendNotification(const User& recipient, const Message &message) override {
        cout<<"[SMS] Sent to "<<recipient.getPhoneNumber()
        << " -> Content: \""<<message.getContent() <<"\"\n";    
    }
};


class InAppNotificationStrategy: public INotificationStrategy {
public:
    void sendNotification(const User& recipient, const Message &message) override {
        cout<<"[IN-APP] Pushed to UserID "<<recipient.getId()
        << " -> Content: \""<<message.getContent() <<"\"\n";    
    }
};


class NotificationStrategyFactory {
public:
    static shared_ptr<INotificationStrategy> getStrategy(NotificationChannelType channel) {
        switch(channel) {
            case NotificationChannelType::EMAIL:
                return make_shared<EmailNotificationStrategy>();
            case NotificationChannelType::SMS:
                return make_shared<SMSNotificationStrategy>();
            case NotificationChannelType::IN_APP:
                return make_shared<InAppNotificationStrategy>();
            default:
                return nullptr;
        }
    }
};




//user & group registeries
class UserRegistry {
private:
    unordered_map<string, User> users;
    
public:
    void addUser(const User& user){
        users.insert({user.getId(), user});
    }

    bool getUser(const string& userId, User& outUser) const {
        auto it = users.find(userId);
        if(it!=users.end()){
            outUser = it->second;
            return true;
        }
        return false;
    }
};


class GroupRegistry {
private:
    std::unordered_map<std::string, Group> groups;

public:
    void addGroup(const Group& group) {
        groups.insert({group.getGroupId(), group});
    }

    bool getGroup(const std::string& groupId, Group& outGroup) const {
        auto it = groups.find(groupId);
        if (it != groups.end()) {
            outGroup = it->second;
            return true;
        }
        return false;
    }
};



// ---Observer Interface --
class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void onMessageSent(const Message& message) = 0;
};

//concrete observer (the notification engine)
class NotificationEngine: public IObserver {
private:
    UserRegistry& userRegistry;
    GroupRegistry& groupRegistry;
    void dispatchToUser(const User &recipient, const Message &message) {
        for(const NotificationChannelType& channel: recipient.getPreferredChannels()){
            auto strategy = NotificationStrategyFactory::getStrategy(channel);
            if(strategy) {
                strategy->sendNotification(recipient, message);
            }
        }
    }
    //
public:
    NotificationEngine(UserRegistry& uReg, GroupRegistry& gReg)
        : userRegistry(uReg), groupRegistry(gReg) {}
    
        void onMessageSent(const Message& message) override {
            if(message.getType() == MessageType::DIRECT){
                User recipient("", "", "", "", {});
                if(userRegistry.getUser(message.getRecipientId(), recipient)) {
                    dispatchToUser(recipient, message);
                }
            } else if (message.getType() == MessageType::GROUP) {
                Group group("", "", {});
                if(groupRegistry.getGroup(message.getRecipientId(), group)) {
                    for(const auto& memberId: group.getMemberUserIds()) {
                        if(memberId == message.getSenderId()) continue;

                        User recipient("", "", "", "", {});
                        if(userRegistry.getUser(memberId, recipient)) {
                            dispatchToUser(recipient, message);
                        }
                    }
                }
            }
        }
};
