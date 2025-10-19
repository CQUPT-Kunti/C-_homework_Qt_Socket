#ifndef SERVICE_MODULE_H
#define SERVICE_MODULE_H

#include <cstddef> // size_t
#include <string>
#include <iostream>
#include <sys/socket.h>

#include "NetworkModule.h"

namespace Service
{

    /**
     * @brief 业务逻辑模块类
     *
     * 负责：
     * - 处理网络模块收到的数据
     * - 当前阶段只打印数据，后续可扩展
     */
    class ServiceModule
    {
    private:
        Net::NetworkModule *network_;

    public:
        explicit ServiceModule(Net::NetworkModule *net);
        ~ServiceModule();

        /**
         * @brief 处理来自客户端的数据
         *
         * @param client_fd 客户端 socket fd
         * @param data 数据缓冲区
         * @param len 数据长度
         */
        void onMessage(int client_fd, const char *data, size_t len);

        /**
         * @brief 可选：向客户端发送消息
         *
         * 需要依赖网络模块提供 sendData 接口
         * @param client_fd 客户端 socket fd
         * @param message 待发送内容
         */
        void sendMessage(int client_fd, const std::string &message);
    };

} // namespace Service

#endif // SERVICE_MODULE_H
