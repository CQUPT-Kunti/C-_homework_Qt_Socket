#include <iostream>
#include "NetworkModule.h"
#include "ServiceModule.h"
int main(int, char **)
{
    Net::NetworkModule *network_ = new Net::NetworkModule();
    Service::ServiceModule *service_ = new Service::ServiceModule(network_);

    service_->start();

    std::cout << "[Main] Running event loop...\n";
    network_->run();

    return 0;
}
