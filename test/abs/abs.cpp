#include "ProcessApp.h"
#include <iostream>
#include <string>
#include <sstream>
#include <regex>

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

void printInforCar(const Infor_Car &value)
{
    std::cout << "{" << value.speed << ", "
              << value.fuelLevel << ", "
              << (value.isAirCondition ? "true" : "false") << "}";
}

int main()
{
    ProcessApp abs("abs");
    abs.receivedNotify();

    std::string input;
    while (true)
    {
        std::getline(std::cin, input);

        std::istringstream iss(input);
        std::string command, argument, data;
        iss >> command >> argument;

        if (command == "SETDATAAPP" && !argument.empty())
        {
            std::getline(iss >> std::ws, data);

            Infor_Car carData;
            if (parseInforCar(data, carData))
            {
                abs.setDataFromClient(argument, carData);
                printInforCar(carData);
                std::cout << std::endl;
            }
            else
            {
                try
                {
                    int intValue = std::stoi(data);
                    abs.setDataFromClient(argument, intValue);
                }
                catch (const std::invalid_argument &)
                {
                    if (!data.empty())
                    {
                        abs.setDataFromClient(argument, data);
                    }
                    else
                    {
                        std::cerr << "Invalid input. Please provide a valid value.\n";
                    }
                }
            }
        }
        else if (command == "SUB" && !argument.empty())
        {
            abs.subscribeTo(argument);
        }
        else if (command == "GETDATA" && !argument.empty())
        {
            std::string dataType;
            iss >> dataType;

            if (dataType == "int")
            {
                int intValue = abs.getData<int>(argument);
            }
            else if (dataType == "string")
            {
                std::string stringValue = abs.getData<std::string>(argument);
            }
            else if (dataType == "struct")
            {
                Infor_Car carData = abs.getData<Infor_Car>(argument);

            }
            else
            {
                std::cerr << "Invalid data type. Please use 'int', 'string', or 'struct'.\n";
            }
        }
        if (command == "SETDATA")
        {
            std::string data = input.substr(8);
            Infor_Car myCar;
            if (parseInforCar(data, myCar))
            {
                abs.setData(myCar);
            }
            else
            {
                try
                {
                    int intValue = std::stoi(data);
                    abs.setData(intValue);
                }
                catch (const std::invalid_argument &)
                {
                    if (!data.empty())
                    {
                        abs.setData(data);
                    }
                    else
                    {
                        std::cerr << "Invalid input. Please provide a valid value.\n";
                    }
                }
            }
        }
        else if (command == "UNSUB" && !argument.empty())
        {
            abs.unsubscribeTo(argument);
        }
    }

    return 0;
}
