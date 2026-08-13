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

