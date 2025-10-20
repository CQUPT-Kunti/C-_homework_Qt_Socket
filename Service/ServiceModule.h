#ifndef SERVICE_MODULE_H
#define SERVICE_MODULE_H

#include <cstddef> // size_t
#include <string>
#include <iostream>
#include <sys/socket.h>

#include "NetworkModule.h"
#include "MultiUserReassembler.h"
#include "ProtocolModule.h"

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
        Protocol::MultiUserReassembler reassembler_;

    public:
        explicit ServiceModule(Net::NetworkModule *net);
        ~ServiceModule();

        void start();

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
         * @param userId  用户id
         * @param type   传输类型
         * @param data   传输数据
         * @param len    消息长度
         *
         */
        void sendMessage(int client_fd, uint32_t userId,
                         Protocol::MsgType type, const char *data, size_t len);

        /**
         * @brief 发送响应消息
         *
         * 需要依赖网络模块提供 sendData 接口
         * @param client_fd 客户端 socket fd
         * @param userId  用户id
         * @param string  发送数据
         *
         */
        void sendResponse(int client_fd, uint32_t userId, const std::string &text);
    };

} // namespace Service

#endif // SERVICE_MODULE_H
