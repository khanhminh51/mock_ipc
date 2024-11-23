#include "DataHandler.h"
#include <sstream>
#include <stdexcept>

std::string DataHandler::serialize_data(const multiData& data) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            oss << "int:" << arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "string:" << arg;
        } else if constexpr (std::is_same_v<T, Infor_Car>) {
            oss << "Infor_Car:" 
                << arg.speed << "," 
                << arg.fuelLevel << "," 
                << arg.isAirCondition;
        } else {
            throw std::runtime_error("Unsupported type for serialization");
        }
    }, data);
    return oss.str();
}

multiData DataHandler::deserialize_data(const std::string& serializedData) {
    std::istringstream iss(serializedData);
    std::string type;
    getline(iss, type, ':');

    if (type == "int") {
        int value;
        iss >> value;
        return value;
    } else if (type == "string") {
        std::string value;
        getline(iss, value);
        return value;
    } else if (type == "Infor_Car") {
        Infor_Car value;
        char delimiter;
        iss >> value.speed >> delimiter 
            >> value.fuelLevel >> delimiter 
            >> value.isAirCondition;
        return value;
    } else {
        throw std::runtime_error("Unknown type in serialized data");
    }
}
