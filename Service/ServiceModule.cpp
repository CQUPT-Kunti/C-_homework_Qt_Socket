#include "ServiceModule.h"
#include "NetworkModule.h"

#include <functional>

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

    void ServiceModule::sendMessage(int client_fd, const std::string &message)
    {
        if (network_ && network_->sendData(client_fd, message.c_str(), message.size()))
        {
            std::cout << "[SUCCESS] Sent: " << message << std::endl;
        }
        else
        {
            std::cout << "[FAILED] Send" << std::endl;
        }
    }

}