#ifndef SERVER_H
#define SERVER_H

#define GREEN "\033[32m"
#define RED "\033[31m"
#define RESET "\033[0m"

#include "Message.h"
#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <unordered_set>
#include <mqueue.h>
// multi_thread
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

#define MAX_MAXMSG 100
#define MAX_MSGSIZE 256
#define SERVER_QUEUE "/sever_queue"

class Server
{
private:
    std::map<std::string, std::unordered_set<std::string>> m_subscribers;
    mqd_t serverQueue;

    void createAppQueue(const std::string &appName);
    void notifyDataChanged(const std::string &service, const std::string &data);
    void notifyServiceRemoveForClient(const std::string &service);
    void notifyDataChangedForService(const std::string &serviceName, const std::string &data, const std::string &client);
    void sendDataToApp(const std::string &client, const std::string &data);
    void getDataFromApp(const std::string &service, const std::string &client);
    void subscribeTo(const std::string &service, const std::string &client);
    void unsubscribeTo(const std::string &service, const std::string &client);
    void handleMessage(const std::string &message);
    void workerFunction(); // Worker thread function
                           // Work queue for multi-threading
    std::queue<std::string> workQueue;
    std::mutex queueMutex;
    std::condition_variable queueCondVar;
    std::vector<std::thread> workers;
    std::atomic<bool> running; // Flag to stop threads
    bool isShutDown{false};

public:
    Server();
    void run();
    void shutdown();
    ~Server();
};

#endif // SERVER_H
