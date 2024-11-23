#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
class Message
{
public:
    enum MessageType
    {
        CHANGED,
        GETDATA,
        RECEIVE,
        SUBSCRIBE,
        NOTEXIST,
        DONEUNSUB,
        CLIENTUNSUB,
        REQUIREDSETDATA,
        CLOSE,
        SHUTDOWN,
        UNKNOWN
    };

    static MessageType getMessageType(const std::string &message);
};
#endif // MESSAGETYPE_H
