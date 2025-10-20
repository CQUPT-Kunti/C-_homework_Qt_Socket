#include "ServiceModule.h"
#include "NetworkModule.h"

#include <functional>
#include <cstring>
#include <chrono>

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
        if (len < sizeof(Protocol::ProtocolHeader))
            return;
        Protocol::ProtocolHeader header{};
        std::memcpy(&header, data, sizeof(Protocol::ProtocolHeader));

        const char *payload = data + sizeof(Protocol::ProtocolHeader);
        size_t payloadLen = len - sizeof(Protocol::ProtocolHeader);

        std::vector<char> fullData;
        bool complete = reassembler_.append(header, payload, fullData);
        if (complete)
        {
            std::cout << "[Service] Received complete msg from user "
                      << header.userId << ", type=" << header.type
                      << ", len=" << fullData.size() << std::endl;
            if (header.type == Protocol::TEXT)
            {
                std::string text(fullData.begin(), fullData.end());
                std::string response = "Server received your message: " + text;
                std::cout << "Text: " << text << std::endl;
                sendResponse(client_fd, header.userId, response);
            }
            else if (header.type == Protocol::IMAGE)
            {
                std::cout << "[IMAGE] Received image of size " << fullData.size() << " bytes" << std::endl;
                std::string response = "Server received your image (" + std::to_string(fullData.size()) + " bytes)";

                sendResponse(client_fd, header.userId, response);
            }
        }
    }

    void ServiceModule::sendResponse(int client_fd, uint32_t userId, const std::string &text)
    {
        sendMessage(client_fd, userId, Protocol::TEXT, text.data(), text.size());
    }

    void ServiceModule::sendMessage(int client_fd, uint32_t userId,
                                    Protocol::MsgType type, const char *data, size_t len)
    {
        Protocol::ProtocolHeader header{};
        // 时间戳 生成msgId
        uint64_t msgId = static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
        auto chunks = Protocol::ProtocolMessage::splitMessage(userId, msgId, type, data, len);
        for (auto &msg : chunks)
        {
            auto buf = Protocol::ProtocolMessage::serializeMessage(msg);
            network_->sendData(client_fd, buf.data(), buf.size());
        }
    }

    void ServiceModule::start()
    {
        network_->init(8081, [&](int fd, const char *data, size_t len)
                       { onMessage(fd, data, len); });
    }

}