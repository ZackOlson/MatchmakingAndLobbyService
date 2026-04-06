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

std::string Serialize(const Message& msg);
Message Deserialize(const std::string& data);
