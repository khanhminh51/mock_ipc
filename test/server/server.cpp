#include "Server.h"
#include <iostream>
#include <string>
#include <thread>

int main()
{
    Server server;

    // Chạy server trên một thread riêng
    std::thread serverThread([&server]() { server.run(); });
    std::string input;

    while (std::getline(std::cin, input))
    {
        if (input == "SHUTDOWN")
        {
            server.shutdown();
            break;
        }
        else
        {
            std::cout << "Unknown command. Type 'SHUTDOWN' to stop the server.\n";
        }
        std::cout << "Enter 'SHUTDOWN' to stop the server: " << std::endl;
    }

    // Chờ server dừng
    if (serverThread.joinable())
    {
        serverThread.join();
    }

    return 0;
}
