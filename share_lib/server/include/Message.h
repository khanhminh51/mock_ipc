#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
class Message
{
public:
    enum MessageType
    {
        CREATE,
        SUBSCRIBE,
        SETDATA,
        GETDATA,
        SENDDATA,
        UNSUB,
        SETDATAAPP,
        CLOSE,
        UNKNOWN
    };

    static MessageType getMessageType(const std::string &message);
};
#endif // MESSAGETYPE_H
