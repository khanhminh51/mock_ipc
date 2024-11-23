#ifndef PROCESS_APP_H
#define PROCESS_APP_H

#define YELLOW "\033[33m"
#define RED "\033[31m"
#define RESET "\033[0m"
#define GREEN "\033[32m"

#include "DataHandler.h"
#include "Message.h"
#include <string>
#include <iostream>
#include <mqueue.h>
#include <unordered_set>
#include <variant>
#include <sstream>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <chrono>
#include <stdexcept>

#define MAX_MAXMSG 100
#define MAX_MSGSIZE 256
#define SERVER_QUEUE "/sever_queue"

class ProcessApp
{
private:
    std::string m_appName;
    mqd_t appQueue;
    std::unordered_set<std::string> m_subscribedProcesses;
    multiData m_data;
    multiData m_targetData;

    void createAppQueue();
    void removeService(const std::string &service);
    void sendData(const std::string &client);
    void getDataFromServer(const std::string &service, const std::string &client);
    void handleMessage(mqd_t mq);

    // for getdata
    std::mutex m_mutex;
    std::condition_variable m_condVar;
    std::atomic<bool> m_dataReady{false};
    bool isClose{false};

public:
    ProcessApp(const std::string &appName);
    void subscribeTo(const std::string &service);
    void unsubscribeTo(const std::string &service);
    void printSubscribedProcesses() const;
    void setData(const multiData &data);
    void setDataFromClient(const std::string &service, const multiData &data);

    // getdata
    template <typename T>
    T getData(const std::string &service)
    {
        if (m_subscribedProcesses.find(service) == m_subscribedProcesses.end())
        {
            std::cout << "Not subscribed to " << service << std::endl;
            throw std::runtime_error("Application is not subscribed.");
        }

        getDataFromServer(service, m_appName);

        std::unique_lock<std::mutex> lock(m_mutex);
        // m_condVar.wait(lock, [this]
        //                { return m_dataReady.load(); });
        if (!m_condVar.wait_for(lock, std::chrono::seconds(1), [this]
                                { return m_dataReady.load(); }))
        {
            throw std::runtime_error("Timeout: No data received for " + service);
        }

        T result = {};
        std::visit([&result](auto &&arg)
                   {
            if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, T>) {
                result = arg;
            } else {
                throw std::runtime_error("Requested type does not match stored data type");
            } },
                   m_targetData);

        std::visit([](auto &&arg)
                   {
            if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, int>)
            {
                arg = 0;
            } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) {
                arg.clear();
            } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, Infor_Car>) {
                arg = Infor_Car{};
            } },
                   m_targetData);

        m_dataReady = false;
        return result;
    }

    void receivedNotify();
    void close();
    ~ProcessApp();
};

#endif // PROCESS_APP_H
