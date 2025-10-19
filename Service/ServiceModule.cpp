#include "ServiceModule.h"
#include "NetworkModule.h"

#include <functional>
#include <cstring>

namespace Service
{
    ServiceModule::ServiceModule(Net::NetworkModule *net)
        : network_(net)
    {
        std::cout << "[ServiceModule] Initialized with NetworkModule\n";
    }
    ServiceModule::~ServiceModule() {};

    // handle message
    void ServiceModule::onMessage(int client_fd, const char *data, size_t len)
    {
        std::string msg(data, len);
        std::cout << "[Service] Received from fd " << client_fd << ": " << msg << std::endl;
    }

    void ServiceModule::sendMessage(int client_fd, const char *data, size_t len)
    {
        // 使用 std::string 管理内存
        std::string msg(data, len);
        std::reverse(msg.begin(), msg.end());

        if (network_ && network_->sendData(client_fd, msg.data(), msg.size()))
        {
            std::cout << "[SUCCESS] Sent: " << msg << std::endl;
        }
        else
        {
            std::cout << "[FAILED] Send" << std::endl;
        }
    }

    void ServiceModule::start()
    {
        network_->init(8081, [&](int fd, const char *data, size_t len)
                       { 
                        onMessage(fd, data, len); 
                        sendMessage(fd,data,len); });
    }

}