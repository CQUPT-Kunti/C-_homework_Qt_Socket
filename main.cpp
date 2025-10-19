#include <iostream>
#include "NetworkModule.h"
#include "ServiceModule.h"
int main(int, char **)
{
    Net::NetworkModule *network_ = new Net::NetworkModule();
    Service::ServiceModule *service_ = new Service::ServiceModule(network_);

    if (!network_->init(8081, [&](int fd, const char *data, size_t len)
                        { service_->onMessage(fd, data, len); }))
    {
        std::cerr << "[Error] Failed to init network module.\n";
        return 1;
    }

    std::cout << "[Main] Running event loop...\n";
    network_->run();

    return 0;
}
