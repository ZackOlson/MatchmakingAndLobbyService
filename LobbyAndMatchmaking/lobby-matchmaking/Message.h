#pragma once
#include <string>
#include <vector>

enum class MessageType 
{
    AUTH_REQUEST,
    AUTH_RESPONSE,
    JOIN_QUEUE,
    QUEUE_UPDATE,
    MATCH_FOUND,
    LOBBY_UPDATE,
    CHAT_MESSAGE
};

struct Message 
{
    MessageType type;
    std::string senderId;
    std::string content; // JSON string for now
};

std::string Serialize(const Message& msg)
{
    return std::to_string((int)msg.type) + "|" + msg.senderId + "|" + msg.content;
}

Message Deserialize(const std::string& data)
{
    Message msg;
    size_t p1 = data.find("|");
    size_t p2 = data.find("|", p1 + 1);

    msg.type = (MessageType)std::stoi(data.substr(0, p1));
    msg.senderId = data.substr(p1 + 1, p2 - p1 - 1);
    msg.content = data.substr(p2 + 1);

    return msg;
}
