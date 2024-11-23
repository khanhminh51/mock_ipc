#include "ProcessApp.h"
#include <iostream>
#include <string>
#include <sstream>
#include <regex>

// Hàm phân tích cú pháp của Infor_Car từ chuỗi
bool parseInforCar(const std::string &input, Infor_Car &value)
{
    std::regex structRegex(R"(\{\s*(\d+)\s*,\s*([\d.]+)\s*,\s*(true|false)\s*\})");
    std::smatch match;

    if (std::regex_search(input, match, structRegex))
    {
        try
        {
            value.speed = std::stoi(match[1]);
            value.fuelLevel = std::stod(match[2]);
            value.isAirCondition = (match[3] == "true");

            return true;
        }
        catch (...)
        {
            return false;
        }
    }
    return false;
}

int main()
{
    ProcessApp ecu("ecu");
    ecu.receivedNotify();

    std::string input;

    while (std::getline(std::cin, input))
    {
        std::istringstream iss(input);
        std::string command;

        if (iss >> command)
        {
            if (command == "SETDATA")
            {
                std::string data = input.substr(8);
                Infor_Car myCar;
                if (parseInforCar(data, myCar))
                {
                    ecu.setData(myCar);
                }
                else
                {
                    try
                    {
                        int intValue = std::stoi(data);
                        ecu.setData(intValue);
                    }
                    catch (const std::invalid_argument &)
                    {
                        if (!data.empty())
                        {
                            ecu.setData(data);
                        }
                        else
                        {
                            std::cerr << "Invalid input. Please provide a valid value.\n";
                        }
                    }
                }
            }

            else if (command == "CLOSE")
            {
                ecu.close();
            }
            else
            {
                std::cerr << "Unknown command. Please use SETDATA <value> or REMOVE.\n";
            }
        }
        else
        {
            std::cerr << "Invalid input. Please use SETDATA <value>.\n";
        }
    }

    return 0;
}
