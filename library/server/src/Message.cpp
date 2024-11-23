#include "../include/Message.h"

Message::MessageType Message::getMessageType(const std::string &message)
{
    size_t firstSpace = message.find(' ');
    std::string messageType = (firstSpace != std::string::npos) ? message.substr(0, firstSpace) : message;

    if (messageType == "CREATE")
        return MessageType::CREATE;
    if (messageType == "SUBSCRIBE")
        return MessageType::SUBSCRIBE;
    if (messageType == "SETDATA")
        return MessageType::SETDATA;
    if (messageType == "GETDATA")
        return MessageType::GETDATA;
    if (messageType == "SENDDATA")
        return MessageType::SENDDATA;
    if (messageType == "UNSUB")
        return MessageType::UNSUB;
    if (messageType == "SETDATAAPP")
        return MessageType::SETDATAAPP;
    if (messageType == "CLOSE")
        return MessageType::CLOSE;

    return MessageType::UNKNOWN;
}
