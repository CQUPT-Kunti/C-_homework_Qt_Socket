#include "NetworkModule.h"
#include "ProtocolModule.h"

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <map>
#include <vector>
#include <cstring>

namespace Net
{

    struct NetworkModule::Impl
    {
        event_base *base;                             // libevent 事件循环
        evconnlistener *listener;                     // TCP 监听器
        std::map<int, bufferevent *> clients;         // 客户端连接表
        std::map<int, std::vector<char>> recvBuffers; // 每个客户端的缓存区
        OnDataCallback callback;                      // 数据到达回调
    };

    // 输入缓冲区有数据可读
    static void read_back(bufferevent *bev, void *ctx)
    {
        NetworkModule::Impl *impl = static_cast<NetworkModule::Impl *>(ctx);
        int fd = bufferevent_getfd(bev);
        char buf[8192];

        // 读数据并累积到对应 fd 的缓存里
        while (true)
        {
            int n = bufferevent_read(bev, buf, sizeof(buf));
            if (n <= 0)
                break;
            auto &buffer = impl->recvBuffers[fd];
            buffer.insert(buffer.end(), buf, buf + n);

            // 尝试拆包：先读协议头，确定整包长度，再循环处理完整包
            while (buffer.size() >= sizeof(Protocol::ProtocolHeader))
            {
                Protocol::ProtocolHeader hdr;
                std::memcpy(&hdr, buffer.data(), sizeof(hdr));
                size_t packetLen = sizeof(hdr) + hdr.payloadLen;
                if (buffer.size() < packetLen)
                    break;

                // 回调一个完整分片
                if (impl->callback)
                {
                    impl->callback(fd, buffer.data(), packetLen);
                }
                // 移除已处理的数据
                buffer.erase(buffer.begin(), buffer.begin() + packetLen);
            }
        }
    }

    // 连接关闭、出错或超时
    static void event_back(bufferevent *bev, short events, void *ctx)
    {
        NetworkModule::Impl *impl = static_cast<NetworkModule::Impl *>(ctx);
        int fd = bufferevent_getfd(bev);
        if (events & BEV_EVENT_ERROR)
        {
            std::cout << "[ERROR] Client disconnected: fd=" << fd << std::endl;
        }
        else if (events & BEV_EVENT_TIMEOUT)
        {
            std::cout << "[ERROR] TIME OUT: fd=" << fd << std::endl;
        }
        bufferevent_free(bev);
        impl->clients.erase(fd);
        impl->recvBuffers.erase(fd);
    }

    // 新客户端连接回调
    static void listener_cb(evconnlistener *listener, evutil_socket_t fd,
                            sockaddr *addr, int socklen, void *ctx)
    {
        NetworkModule::Impl *impl = static_cast<NetworkModule::Impl *>(ctx);
        bufferevent *bev = bufferevent_socket_new(impl->base, fd, BEV_OPT_CLOSE_ON_FREE);
        bufferevent_setcb(bev, read_back, nullptr, event_back, impl);
        bufferevent_enable(bev, EV_READ | EV_WRITE);
        impl->clients[fd] = bev;
        char ip[INET_ADDRSTRLEN] = {0};
        sockaddr_in *sin = reinterpret_cast<sockaddr_in *>(addr);
        inet_ntop(AF_INET, &sin->sin_addr, ip, sizeof(ip));
        std::cout << "New client connected: fd=" << fd << ", ip=" << ip << std::endl;
    }

    NetworkModule::NetworkModule()
    {
        impl_ = new Impl();
    }

    NetworkModule::~NetworkModule()
    {
        shutdown();
        delete impl_;
    }

    bool NetworkModule::init(int port, OnDataCallback cb)
    {
        impl_->callback = cb;
        impl_->base = event_base_new();
        if (!impl_->base)
            return false;

        sockaddr_in sin{};
        sin.sin_family = AF_INET;
        sin.sin_port = htons(port);
        sin.sin_addr.s_addr = INADDR_ANY;

        impl_->listener = evconnlistener_new_bind(
            impl_->base, listener_cb, impl_,
            LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
            -1, reinterpret_cast<sockaddr *>(&sin), sizeof(sin));
        return impl_->listener != nullptr;
    }

    void NetworkModule::run()
    {
        if (impl_->base)
        {
            event_base_dispatch(impl_->base);
        }
    }

    void NetworkModule::shutdown()
    {
        if (impl_->listener)
        {
            evconnlistener_free(impl_->listener);
            impl_->listener = nullptr;
        }
        for (auto &[fd, bev] : impl_->clients)
        {
            bufferevent_free(bev);
        }
        impl_->clients.clear();
        impl_->recvBuffers.clear();
        if (impl_->base)
        {
            event_base_free(impl_->base);
            impl_->base = nullptr;
        }
    }

    bool NetworkModule::sendData(int client_fd, const char *data, size_t len)
    {
        auto it = impl_->clients.find(client_fd);
        if (it == impl_->clients.end())
            return false;
        bufferevent_write(it->second, data, len);
        return true;
    }

    NetworkModule::Impl *NetworkModule::getImpl()
    {
        return impl_;
    }

} // namespace Net
