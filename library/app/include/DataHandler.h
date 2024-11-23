#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include <string>
#include <variant>

/* Information in car*/
struct Infor_Car
{
    int speed;
    double fuelLevel;
    bool isAirCondition;
    Infor_Car() : speed(0), fuelLevel(0.0), isAirCondition(false) {}
};

using multiData = std::variant<int, std::string, Infor_Car>;

class DataHandler
{
public:
    static std::string serialize_data(const multiData &data);
    static multiData deserialize_data(const std::string &serializedData);
};

#endif // DATA_HANDLER_H
