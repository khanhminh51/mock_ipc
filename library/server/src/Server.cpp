#include "Server.h"

Server::Server() : running(true)
{
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MAXMSG;
    attr.mq_msgsize = MAX_MSGSIZE;
    attr.mq_curmsgs = 0;
    serverQueue = mq_open(SERVER_QUEUE, O_CREAT | O_RDONLY, 0660, &attr);
    if (serverQueue == -1)
    {
        std::cerr << RED << "[Server] Failed to open server queue" << strerror(errno) << std::endl;
    }
    // Start worker threads
    for (int i = 0; i < std::thread::hardware_concurrency(); ++i)
    {
        workers.emplace_back(&Server::workerFunction, this);
    }
}

void Server::createAppQueue(const std::string &appName)
{
    m_subscribers[appName] = std::unordered_set<std::string>();
    std::cout << GREEN << "[Server] Created queue for: " << appName << std::endl;
}

void Server::subscribeTo(const std::string &service, const std::string &client)
{
    // Kiểm tra xem targetApp có tồn tại trong map không
    auto it = m_subscribers.find(service);
    if (it != m_subscribers.end())
    {
        // Nếu có, thêm subscriber vào unordered_set của targetApp
        it->second.insert(client);
        std::cout << GREEN << "[Server] " << client << " subscribed to " << service << std::endl;
        mqd_t mq = mq_open(("/" + service).c_str(), O_WRONLY);
        if (mq != -1)
        {
            std::string message = "SUBSCRIBE " + client;
            mq_send(mq, message.c_str(), message.size() + 1, 0);
            mq_close(mq);
        }
        else
        {
            std::cerr << RED << "[Server] Failed to open queue with: " << service << std::endl;
        }
    }
    else
    {
        // Nếu không có, báo lỗi
        std::cerr << RED << "[Server] Error: Service '" << service << "' does not exist. Subscription failed." << std::endl;
        mqd_t mq = mq_open(("/" + client).c_str(), O_WRONLY);
        if (mq != -1)
        {
            std::string message = "NOTEXIST " + service;
            mq_send(mq, message.c_str(), message.size() + 1, 0);
            mq_close(mq);
        }
        else
        {
            std::cerr << RED << "[Server] Failed to open queue with: " << client << std::endl;
        }
    }
}

void Server::unsubscribeTo(const std::string &service, const std::string &client)
{
    m_subscribers[service].erase(client);
    mqd_t subscriberQueue = mq_open(("/" + client).c_str(), O_WRONLY);
    if (subscriberQueue != -1)
    {
        std::string message = "DONEUNSUB " + service;
        mq_send(subscriberQueue, message.c_str(), message.size() + 1, 0);
        mq_close(subscriberQueue);
        std::cout << GREEN << client << " just unsubscribe to " << service << std::endl;
    }
    mqd_t serviceQueue = mq_open(("/" + service).c_str(), O_WRONLY);
    if (serviceQueue != -1)
    {
        std::string message = "CLIENTUNSUB " + client;
        mq_send(serviceQueue, message.c_str(), message.size() + 1, 0);
        mq_close(serviceQueue);
    }
}

void Server::notifyDataChanged(const std::string &service, const std::string &data)
{
    auto it = m_subscribers.find(service);
    if (it != m_subscribers.end())
    {
        for (const auto &subscriber : it->second)
        {
            mqd_t mq = mq_open(("/" + subscriber).c_str(), O_WRONLY);
            if (mq != -1)
            {
                std::string message = "CHANGED " + service + " has changed data to: " + data;
                mq_send(mq, message.c_str(), message.size() + 1, 0);
                mq_close(mq);
            }
            else
            {
                std::cerr << RED << "[Server] Failed to open queue for subscriber " << subscriber << std::endl;
            }
        }
    }
    std::cout << GREEN << "[Server] CHANGED data from " << service << ": " << data << std::endl;
}

void Server::notifyServiceRemoveForClient(const std::string &service)
{
    auto it = m_subscribers.find(service);
    if (it != m_subscribers.end())
    {
        for (const auto &subscriber : it->second)
        {
            mqd_t mq = mq_open(("/" + subscriber).c_str(), O_WRONLY);
            if (mq != -1)
            {
                std::string message = "CLOSE " + service;
                mq_send(mq, message.c_str(), message.size() + 1, 0);
                mq_close(mq);
            }
            else
            {
                std::cerr << RED << "[Server] Failed to open queue for subscriber " << subscriber << std::endl;
            }
        }
        m_subscribers.erase(it);
    }
    std::cout << GREEN << "[Server]" << service << " just remove from Server" << std::endl;
}

void Server::notifyDataChangedForService(const std::string &serviceName, const std::string &data, const std::string &client)
{
    mqd_t mq = mq_open(("/" + serviceName).c_str(), O_WRONLY);

    if (mq != -1)
    {
        std::string message = "REQUIREDSETDATA " + data + " " + client;
        mq_send(mq, message.c_str(), message.size() + 1, 0);
        mq_close(mq);
        std::cout << GREEN << "[Server] Required set data on " << serviceName << std::endl;
        notifyDataChanged(serviceName, data);
    }
}

void Server::getDataFromApp(const std::string &service, const std::string &client)
{
    // std::cout << "[SERVER] | GETDATA | STEP 5 \n";

    mqd_t mq = mq_open(("/" + service).c_str(), O_WRONLY);
    if (mq != -1)
    {
        std::string message = "GETDATA " + client;
        mq_send(mq, message.c_str(), message.size() + 1, 0);
        mq_close(mq);
        std::cout << GREEN << "[Server] Get data from " << service << std::endl;
        // std::cout << "[SERVER] | GETDATA | STEP 6 \n";
    }
    else
    {
        std::cerr << RED << "[Server] Failed to get data from " << service << std::endl;
    }
}

void Server::sendDataToApp(const std::string &client, const std::string &data)
{
    // std::cout << "[SERVER] | GETDATA | STEP 11 \n";
    mqd_t mq = mq_open(("/" + client).c_str(), O_WRONLY);
    if (mq != -1)
    {
        std::string message = "RECEIVE " + data;
        mq_send(mq, message.c_str(), message.size() + 1, 0);
        mq_close(mq);
        std::cout << GREEN << "[Server] SEND data to " << client << std::endl;
        // std::cout << "[SERVER] | GETDATA | STEP 12 \n";
    }
    else
    {
        std::cerr << RED << "[Server] Failed SEND data to " << client << std::endl;
    }
}

void Server::handleMessage(const std::string &message)
{
    Message::MessageType messageType = Message::getMessageType(message);
    size_t firstSpace = message.find(' ');

    switch (messageType)
    {
    case Message::MessageType::CREATE:
    {
        std::string appName = message.substr(7);
        createAppQueue(appName);
        break;
    }
    case Message::MessageType::SUBSCRIBE:
    {
        std::string service = message.substr(firstSpace + 1, message.find(' ', firstSpace + 1) - firstSpace - 1);
        std::string client = message.substr(message.find_last_of(' ') + 1);
        subscribeTo(service, client);
        break;
    }
    case Message::MessageType::SETDATA:
    {
        std::string service = message.substr(firstSpace + 1, message.find(' ', firstSpace + 1) - firstSpace - 1);
        std::string data = message.substr(message.find_last_of(' ') + 1);
        notifyDataChanged(service, data);
        break;
    }
    case Message::MessageType::GETDATA:
    {
        std::string service = message.substr(firstSpace + 1, message.find(' ', firstSpace + 1) - firstSpace - 1);
        std::string client = message.substr(message.find_last_of(' ') + 1);
        getDataFromApp(service, client);
        break;
    }
    case Message::MessageType::SENDDATA:
    {
        std::string client = message.substr(firstSpace + 1, message.find(' ', firstSpace + 1) - firstSpace - 1);
        std::string data = message.substr(message.find_last_of(' ') + 1);
        sendDataToApp(client, data);
        break;
    }
    case Message::MessageType::UNSUB:
    {
        std::string service = message.substr(firstSpace + 1, message.find(' ', firstSpace + 1) - firstSpace - 1);
        std::string client = message.substr(message.find_last_of(' ') + 1);
        unsubscribeTo(service, client);
        break;
    }
    case Message::MessageType::SETDATAAPP:
    {
        size_t secondSpace = message.find(' ', firstSpace + 1);
        size_t lastSpace = message.find_last_of(' ');
        std::string service = message.substr(firstSpace + 1, secondSpace - firstSpace - 1);
        std::string data = message.substr(secondSpace + 1, lastSpace - secondSpace - 1);
        std::string client = message.substr(lastSpace + 1);
        notifyDataChangedForService(service, data, client);
        break;
    }
    case Message::MessageType::CLOSE:
    {
        std::string service = message.substr(6);
        notifyServiceRemoveForClient(service);
        break;
    }
    default:
        std::cerr << "[Server] Unknown message type: " << message << std::endl;
        break;
    }
}

void Server::workerFunction()
{
    while (running)
    {
        std::string message;

        // Wait for a message to process
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondVar.wait(lock, [this]()
                              { return !workQueue.empty() || !running; });

            if (!running && workQueue.empty())
            {
                break;
            }

            message = workQueue.front();
            workQueue.pop();
        }

        // Process the message
        handleMessage(message);
    }
}

void Server::run()
{
    std::cout << GREEN << "Server is running..." << std::endl;

    struct timespec timeout;
    while (running)
    {
        // Thiết lập thời gian timeout
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_sec += 1; // Chờ tối đa 1 giây

        char buffer[MAX_MSGSIZE];
        ssize_t bytesRead = mq_timedreceive(serverQueue, buffer, MAX_MSGSIZE, nullptr, &timeout);
        if (bytesRead >= 0)
        {
            buffer[bytesRead] = '\0';
            std::string message(buffer);

            // Đẩy message vào hàng đợi công việc
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                workQueue.push(message);
            }
            queueCondVar.notify_one();
        }
        else if (errno == ETIMEDOUT)
        {
            // Không có message sau timeout, tiếp tục kiểm tra điều kiện vòng lặp
            continue;
        }
        else if (!running)
        {
            break;
        }
        else
        {
            std::cerr << RED << "[Server] Failed to receive message: " << strerror(errno) << std::endl;
        }
    }

    std::cout << GREEN << "[Server] Stopping run loop." << std::endl;
}

void Server::shutdown()
{

    std::unordered_set<std::string> visited;

    for (const auto &[key, values] : m_subscribers)
    {
        // Kiểm tra và xử lý key
        if (visited.find(key) == visited.end())
        {
            visited.insert(key);

            // Mở queue và gửi message
            mqd_t appQueue = mq_open(("/" + key).c_str(), O_WRONLY);
            if (appQueue != -1)
            {
                std::string message = "SHUTDOWN Server Queue";
                mq_send(appQueue, message.c_str(), message.size() + 1, 0);
                mq_close(appQueue);
                std::cout << GREEN << "[Server] Notify Close server queue for " << key << std::endl;
            }
            else
            {
                std::cout << RED << "[Server] Failed notify Close server queue for " << key << std::endl;
            }
        }

        // Kiểm tra và xử lý các phần tử trong unordered_set
        for (const auto &subscriber : values)
        {
            if (visited.find(subscriber) == visited.end())
            {
                visited.insert(subscriber);

                mqd_t appQueue = mq_open(("/" + subscriber).c_str(), O_WRONLY);
                if (appQueue != -1)
                {
                    std::string message = "SHUTDOWN Server Queue";
                    mq_send(appQueue, message.c_str(), message.size() + 1, 0);
                    mq_close(appQueue);
                    std::cout << GREEN << "[Server] Notify Close server queue for " << subscriber << std::endl;
                }
                else
                {
                    std::cout << RED << "[Server] Failed notify Close server queue for " << subscriber << std::endl;
                }
            }
        }
    }
    // destrcutor of server
    // Signal threads to stop
    running = false;
    queueCondVar.notify_all();

    // Join worker threads
    for (auto &worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }

    // Close and unlink server queue
    if (serverQueue != -1)
    {
        mq_close(serverQueue);
    }
    if (mq_unlink(SERVER_QUEUE) == -1)
    {
        std::cerr << RED << "[Server] Failed to unlink server queue: " << strerror(errno) << std::endl;
    }
    std::cout << RED << "Server is shutting down...\n";
    isShutDown = true;
}
Server::~Server()
{
    if (!isShutDown)
    {

        shutdown();
    }
}