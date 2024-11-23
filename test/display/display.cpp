/*Nội dung trong file này khác so với test do vi phạm về bảo mật key của Adafruit nên em thay đổi 1 số chỗ*/

#include "ProcessApp.h"
#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <curl/curl.h>

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
    std::cout << "{" << value.speed << ", " << value.fuelLevel << ", " << (value.isAirCondition ? "true" : "false") << "}";
}

// Hàm gửi dữ liệu lên Adafruit IO
void sendDataToAdafruit(const std::string &feedName, const std::string &value, const std::string &aioKey, const std::string &username)
{
    std::string url = "https://io.adafruit.com/api/v2/" + username + "/feeds/" + feedName + "/data";
    std::cout << "url: " << url << "\n";
    std::string jsonData = "{\"value\": \"" + value + "\"}";

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("X-AIO-Key: " + aioKey).c_str());

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "Failed to send data: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            std::cout << "Data sent to feed " << feedName << " successfully!" << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}

int main()
{
    const std::string username = "minhpham51";
    const std::string aioKey = "";

    ProcessApp display("display");
    display.receivedNotify();

    std::string input;
    while (true)
    {
        std::getline(std::cin, input);

        // Parse the command
        std::istringstream iss(input);
        std::string command, argument, data;
        iss >> command >> argument;

        if (command == "SETDATAAPP" && !argument.empty())
        {
            std::getline(iss >> std::ws, data);

            Infor_Car carInfo;
            if (parseInforCar(data, carInfo))
            {
                display.setDataFromClient(argument, carInfo);
                printInforCar(carInfo);
                std::cout << std::endl;
            }
            else
            {
                try
                {
                    int intValue = std::stoi(data);
                    display.setDataFromClient(argument, intValue);
                }
                catch (const std::invalid_argument &)
                {
                    if (!data.empty())
                    {
                        display.setDataFromClient(argument, data);
                    }
                    else
                    {
                        std::cerr << "Invalid input. Please provide a valid value.\n";
                    }
                }
            }
        }
        if (command == "SETDATA")
        {
            std::string data = input.substr(8);
            Infor_Car myCar;
            if (parseInforCar(data, myCar))
            {
                display.setData(myCar);
            }
            else
            {
                try
                {
                    int intValue = std::stoi(data);
                    display.setData(intValue);
                }
                catch (const std::invalid_argument &)
                {
                    if (!data.empty())
                    {
                        display.setData(data);
                    }
                    else
                    {
                        std::cerr << "Invalid input. Please provide a valid value.\n";
                    }
                }
            }
        }
        else if (command == "GETDATA" && !argument.empty())
        {
            std::string dataType;
            iss >> dataType;

            if (dataType == "struct")
            {
                Infor_Car carInfo = display.getData<Infor_Car>(argument);

                // Gửi dữ liệu struct lên các feed của Adafruit IO
                sendDataToAdafruit("speed", std::to_string(carInfo.speed), aioKey, username);
                sendDataToAdafruit("fuellevel", std::to_string(carInfo.fuelLevel), aioKey, username);
                sendDataToAdafruit("airconditioner", carInfo.isAirCondition ? "true" : "false", aioKey, username);

                std::cout << "Data sent to Adafruit IO: ";
                printInforCar(carInfo);
                std::cout << std::endl;
            }
            else
            {
                std::cerr << "GETDATA with type struct is required for Adafruit IO integration." << std::endl;
            }
        }
        else if (command == "SUB" && !argument.empty())
        {
            display.subscribeTo(argument);
        }
        else if (command == "UNSUB" && !argument.empty())
        {
            display.unsubscribeTo(argument);
        }
        else
        {
            std::cout << "Invalid command. Use 'SUB <ProcessName>', 'GETDATA <ProcessName> struct', 'UNSUB <ProcessName>', or 'SETDATAAPP <ProcessName> <Data>'" << std::endl;
        }
    }

    return 0;
}