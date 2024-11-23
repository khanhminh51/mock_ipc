#include "../include/ProcessApp.h"
#include <cstring>
#include <thread>
#include <chrono>

ProcessApp::ProcessApp(const std::string &appName) : m_appName(appName)
{
    createAppQueue();
}

void ProcessApp::createAppQueue()
{
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MAXMSG;
    attr.mq_msgsize = MAX_MSGSIZE;
    attr.mq_curmsgs = 0;

    appQueue = mq_open(("/" + m_appName).c_str(), O_CREAT | O_RDWR, 0660, &attr);
    if (appQueue == -1)
    {
        std::cerr << RED << "[ProcessApp] Failed to create queue for " << m_appName << std::endl;
    }
    else
    {
        std::cout << GREEN << "[ProcessApp] Created queue for " << m_appName << std::endl;
        mqd_t serverQueue = mq_open(SERVER_QUEUE, O_WRONLY);
        if (serverQueue != -1)
        {
            std::string message = "CREATE " + m_appName;
            if (mq_send(serverQueue, message.c_str(), message.size() + 1, 0) == -1)
            {
                std::cerr << RED << "[ProcessApp] Failed to send CREATE message for " << m_appName << std::endl;
            }
            else
            {
                std::cout << GREEN << "[ProcessApp] Sent CREATE message to server for " << m_appName << std::endl;
            }
            mq_close(serverQueue);
        }
        else
        {
            std::cerr << RED << "[ProcessApp] Failed to open server queue to send CREATE message" << std::endl;
        }
    }
}

void ProcessApp::subscribeTo(const std::string &service)
{
    // Kiểm tra nhanh xem đã đăng ký chưa
    if (m_subscribedProcesses.find(service) != m_subscribedProcesses.end())
    {
        std::cout << GREEN << "Already subscribed to " << service << std::endl;
    }
    else
    {
        mqd_t serverQueue = mq_open(SERVER_QUEUE, O_WRONLY);
        if (serverQueue != -1)
        {
            std::string message = "SUBSCRIBE " + service + " " + m_appName;
            if (mq_send(serverQueue, message.c_str(), message.size() + 1, 0) == -1)
            {
                std::cerr << RED << "[ProcessApp] Failed to send subscribe message for " << m_appName << std::endl;
            }
            else
            {
                std::cout << GREEN << "[ProcessApp] " << m_appName << " subscribed to " << service << std::endl;
                // Thêm targetApp vào danh sách đăng ký
                m_subscribedProcesses.insert(service);
            }
            mq_close(serverQueue);
        }
        else
        {
            std::cerr << RED << "[ProcessApp] Failed to open server queue for subscribing" << std::endl;
        }
    }
}

void ProcessApp::unsubscribeTo(const std::string &service)
{
    // Kiểm tra nhanh xem đã đăng ký chưa
    if (m_subscribedProcesses.find(service) == m_subscribedProcesses.end())
    {
        std::cout << RED << "Not subscribed on " << service << std::endl;
    }
    else
    {
        mqd_t serverQueue = mq_open(SERVER_QUEUE, O_WRONLY);
        if (serverQueue != -1)
        {
            std::string message = "UNSUB " + service + " " + m_appName;
            if (mq_send(serverQueue, message.c_str(), message.size() + 1, 0) == -1)
            {
                std::cerr << RED << "[ProcessApp] Failed to unsubscribe on" << service << std::endl;
            }
            else
            {
                removeService(service);
            }
            mq_close(serverQueue);
        }
        else
        {
            std::cerr << RED << "[ProcessApp] Failed to open server queue for sending data" << std::endl;
        }
    }
}

void ProcessApp::removeService(const std::string &service)
{
    m_subscribedProcesses.erase(service);
    std::cout << GREEN << "[ProcessApp] Unsuscribe on " << service << std::endl;
}

void ProcessApp::printSubscribedProcesses() const
{
    // std::cout << "Subscribed processes for " << m_appName << ": ";
    if (m_subscribedProcesses.empty())
    {
        std::cout << RED << "No subscriptions." << std::endl;
    }
    else
    {
        for (const auto &process : m_subscribedProcesses)
        {
            std::cout << process << " ";
        }
        std::cout << std::endl;
    }
}
void ProcessApp::setData(const multiData &data)
{
    m_data = data;
    mqd_t serverQueue = mq_open(SERVER_QUEUE, O_WRONLY);
    if (serverQueue != -1)
    {
        std::string dataStr = DataHandler::serialize_data(data);
        std::string message = "SETDATA " + m_appName + " " + dataStr;
        if (mq_send(serverQueue, message.c_str(), message.size() + 1, 0) == -1)
        {
            std::cerr << RED << "[ProcessApp] Failed to set data message for " << m_appName << std::endl;
        }
        else
        {
            std::cout << GREEN << "[ProcessApp] " << m_appName << " set data: " << dataStr << std::endl;
        }
        mq_close(serverQueue);
    }
    else
    {
        std::cerr << RED << "[ProcessApp] Failed to open server queue for sending data" << std::endl;
    }
}

void ProcessApp::setDataFromClient(const std::string &service, const multiData &data)
{
    if (m_subscribedProcesses.find(service) == m_subscribedProcesses.end())
    {
        std::cout << RED << "Not subscribed on " << service << std::endl;
    }
    else
    {
        mqd_t serverQueue = mq_open(SERVER_QUEUE, O_WRONLY);
        if (serverQueue != -1)
        {
            std::string dataStr = DataHandler::serialize_data(data);
            std::string message = "SETDATAAPP " + service + " " + dataStr + " " + m_appName;

            if (mq_send(serverQueue, message.c_str(), message.size() + 1, 0) == -1)
            {
                std::cerr << RED << "[ProcessApp] Failed to set data for " << service << std::endl;
            }
            else
            {
                std::cout << GREEN << "[ProcessApp] Set data for " << service << std::endl;
            }
            mq_close(serverQueue);
        }
        else
        {
            std::cerr << RED << "[ProcessApp] Failed to open server queue for set data" << std::endl;
        }
    }
}

void ProcessApp::getDataFromServer(const std::string &service, const std::string &client)
{
    // std::cout << "[PROCESS] | GETDATA | STEP 1 \n";
    mqd_t serverQueue = mq_open(SERVER_QUEUE, O_WRONLY);
    if (serverQueue != -1)
    {
        // std::cout << "[PROCESS] | GETDATA | STEP 2 \n";
        std::string message = "GETDATA " + service + " " + client;
        if (mq_send(serverQueue, message.c_str(), message.size() + 1, 0) == -1)
        {
            std::cerr << RED << "[ProcessApp] Failed to get data from " << service << std::endl;
        }
        else
        {
            std::cout << GREEN << "[ProcessApp] " << client << " get data from: " << service << std::endl;
            // std::cout << "[PROCESS] | GETDATA | STEP 3 \n";
        }
        mq_close(serverQueue);
    }
    else
    {
        std::cerr << RED << "[ProcessApp] Failed to open server queue for sending data" << std::endl;
    }
}

void ProcessApp::sendData(const std::string &client)
{
    // std::cout << "[PROCESS] | GETDATA | STEP 8 \n";
    mqd_t serverQueue = mq_open(SERVER_QUEUE, O_WRONLY);
    if (serverQueue != -1)
    {
        multiData data = m_data;
        std::string dataStr = DataHandler::serialize_data(data);
        std::string message = "SENDDATA " + client + " " + dataStr;
        if (mq_send(serverQueue, message.c_str(), message.size() + 1, 0) == -1)
        {
            std::cerr << RED << "[ProcessApp] Failed to send data message for " << client << std::endl;
        }
        else
        {
            // std::cout << "Data for send: " << dataStr << "\n";
            std::cout << GREEN << "[ProcessApp] " << m_appName << " send data for " << client << std::endl;
            // std::cout << "[PROCESS] | GETDATA | STEP 9 \n";
        }
        mq_close(serverQueue);
    }
    else
    {
        std::cerr << RED << "[ProcessApp] Failed to open server queue for sending data" << std::endl;
    }
}

void ProcessApp::handleMessage(mqd_t mq)
{
    char buffer[256];
    ssize_t bytesRead = mq_receive(mq, buffer, MAX_MSGSIZE, nullptr);
    if (bytesRead >= 0)
    {
        buffer[bytesRead] = '\0';
        std::string message(buffer);
        Message::MessageType messageType = Message::getMessageType(message);
        size_t firstSpace = message.find(' ');

        switch (messageType)
        {
        case Message::MessageType::CHANGED:
            std::cout << GREEN << message.substr(8) << "\n";
            break;

        case Message::MessageType::GETDATA:
            sendData(message.substr(8));
            break;

        case Message::MessageType::RECEIVE:
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_targetData = DataHandler::deserialize_data(message.substr(8));
            m_dataReady = true;
            m_condVar.notify_one();
        }
        break;

        case Message::MessageType::SUBSCRIBE:
            std::cout << GREEN << message.substr(10) << " just subscribed on your app" << std::endl;
            break;

        case Message::MessageType::NOTEXIST:
            removeService(message.substr(9));
            break;

        case Message::MessageType::DONEUNSUB:
            std::cout << GREEN << "[PROCESS] Successful unsubscribe on " + message.substr(10) << std::endl;
            break;

        case Message::MessageType::CLIENTUNSUB:
            std::cout << YELLOW << "[PROCESS] " << message.substr(12) << " just unsubscribe on your app" << std::endl;
            break;

        case Message::MessageType::REQUIREDSETDATA:
        {
            size_t secondSpace = message.find(' ', firstSpace + 1);
            std::string data = message.substr(firstSpace + 1, secondSpace - firstSpace - 1);
            std::string appName = message.substr(message.find_last_of(' ') + 1);
            m_data = DataHandler::deserialize_data(data);
            std::cout << GREEN << "[PROCESS] Successful set data to " + data + " from " << appName << std::endl;
        }
        break;

        case Message::MessageType::CLOSE:
            removeService(message.substr(6));
            break;

        case Message::MessageType::SHUTDOWN:
            std::cout << RED << "Server was shutdown" << std::endl;
            break;

        default:
            std::cerr << RED << "[ProcessApp] Unknown message type: " << messageType << std::endl;
            break;
        }
    }
    else
    {
        std::cerr << RED << "[ProcessApp] Failed to receive notification for " << m_appName << std::endl;
    }
}

void ProcessApp::receivedNotify()
{
    // Tạo một luồng mới để luôn lắng nghe các thông báo
    std::thread([this]()
                {
        appQueue = mq_open(("/" + m_appName).c_str(), O_RDONLY);
        if (appQueue == -1)
        {
            std::cerr << RED << "[ProcessApp] Failed to open queue for receiving notifications in " << m_appName << std::endl;
            return;
        }

        while (true)
        {        
           handleMessage(appQueue);
        }

        mq_close(appQueue); })
        .detach();
}

void ProcessApp::close()
{
    if (appQueue != -1)
    {
        if (mq_close(appQueue) == -1)
        {
            std::cerr << RED << "[ProcessApp] Failed to close app queue for " << m_appName
                      << ": " << strerror(errno) << std::endl;
            return;
        }
        appQueue = -1;
        std::cout << YELLOW << "[ProcessApp] Closed queue for " << m_appName << std::endl;
    }

    if (mq_unlink(("/" + m_appName).c_str()) == -1)
    {
        if (errno != ENOENT) // Bỏ qua nếu hàng đợi không tồn tại
        {
            std::cerr << RED << "[ProcessApp] Failed to unlink queue for " << m_appName
                      << ": " << strerror(errno) << std::endl;
            return;
        }
    }
    else
    {
        std::cout << GREEN << "[ProcessApp] Successfully unlinked queue for " << m_appName << std::endl;
        isClose = true;
    }

    mqd_t serverQueue = mq_open(SERVER_QUEUE, O_WRONLY);
    if (serverQueue != -1)
    {
        std::string message = "CLOSE " + m_appName;
        if (mq_send(serverQueue, message.c_str(), message.size() + 1, 0) == -1)
        {
            std::cerr << RED << "[ProcessApp] Failed to send CLOSE message for " << m_appName
                      << ": " << strerror(errno) << std::endl;
        }
        else
        {
            std::cout << YELLOW << "[ProcessApp] Sent CLOSE message for " << m_appName << std::endl;
        }
        mq_close(serverQueue);
    }
    else
    {
        std::cerr << RED << "[ProcessApp] Failed to open server queue to send CLOSE message" << std::endl;
    }
}

ProcessApp::~ProcessApp()
{
    if (!isClose)
    {

        close();
    }
}
