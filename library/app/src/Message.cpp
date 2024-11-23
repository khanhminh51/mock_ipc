#include "../include/Message.h"

Message::MessageType Message::getMessageType(const std::string &message)
{
    size_t firstSpace = message.find(' ');
    std::string messageType = (firstSpace != std::string::npos) ? message.substr(0, firstSpace) : message;

    if (messageType == "CHANGED")
        return MessageType::CHANGED;
    if (messageType == "GETDATA")
        return MessageType::GETDATA;
    if (messageType == "RECEIVE")
        return MessageType::RECEIVE;
    if (messageType == "SUBSCRIBE")
        return MessageType::SUBSCRIBE;
    if (messageType == "NOTEXIST")
        return MessageType::NOTEXIST;
    if (messageType == "DONEUNSUB")
        return MessageType::DONEUNSUB;
    if (messageType == "CLIENTUNSUB")
        return MessageType::CLIENTUNSUB;
    if (messageType == "REQUIREDSETDATA")
        return MessageType::REQUIREDSETDATA;
    if (messageType == "CLOSE")
        return MessageType::CLOSE;
    if (messageType == "SHUTDOWN")
        return MessageType::SHUTDOWN;
    return MessageType::UNKNOWN;
}
