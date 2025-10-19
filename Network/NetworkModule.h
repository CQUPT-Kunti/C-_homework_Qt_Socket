#ifndef NETWORK_MODULE_H
#define NETWORK_MODULE_H

#include <cstddef>    // size_t
#include <functional> // std::function
#include <map>        // std::map

struct event_base;
struct evconnlistener;
struct bufferevent;

namespace Net
{

    /**
     * @brief 网络模块回调函数类型
     *
     * 当网络模块收到客户端数据时调用该回调，将原始字节流传递给上层业务逻辑。
     *
     * @param client_fd 客户端 socket 描述符，用于标识连接
     * @param data 指向收到的数据缓冲区（原始字节流）
     * @param len 数据长度
     */
    using OnDataCallback = std::function<void(int client_fd, const char *data, size_t len)>;

    /**
     * @brief 网络模块类
     *
     * 封装 libevent，实现 TCP 服务器功能。
     * 负责：
     * - 监听指定端口
     * - 接收客户端连接
     * - 收发数据（字节流）
     * - 将收到的数据通过回调传给业务逻辑模块
     *
     * 注意：
     * - 不处理协议解析，业务层只关心原始数据
     * - 支持向指定客户端发送数据
     */
    class NetworkModule
    {
    public:
        NetworkModule();
        ~NetworkModule();

        /**
         * @brief 初始化网络模块
         *
         * @param port 监听端口号
         * @param cb 当收到客户端数据时调用的回调函数
         * @return true 初始化成功
         * @return false 初始化失败（如端口占用或 libevent 初始化失败）
         *
         * @note 初始化时：
         * - 创建 event_base
         * - 创建 evconnlistener 并绑定 accept 回调
         * - 保存业务回调
         */
        bool init(int port, OnDataCallback cb);

        /**
         * @brief 启动网络事件循环
         *
         * 调用后进入 libevent 事件循环（阻塞当前线程）。
         * 事件循环会自动：
         * - 监听客户端连接
         * - 触发数据读回调
         * - 处理连接断开或错误事件
         */
        void run();

        /**
         * @brief 停止事件循环
         *
         * 安全关闭网络模块：
         * - 终止 event_base_dispatch
         * - 释放相关资源（需结合析构函数或 shutdown）
         */
        void shutdown();

        /**
         * @brief 向指定客户端发送数据
         *
         * @param client_fd 客户端 socket 描述符
         * @param data 待发送的数据缓冲区
         * @param len 数据长度
         * @return true 数据发送成功（放入 bufferevent 输出缓冲区）
         * @return false 发送失败（如客户端连接无效）
         *
         * @note 实现逻辑：
         * - 查找 client_fd 对应的 bufferevent
         * - 调用 bufferevent_write() 写入输出缓冲区
         * - 数据异步发送
         */
        bool sendData(int client_fd, const char *data, size_t len);

        struct Impl;

        struct Impl *getImpl();

    private:
        /**
         * @brief 内部实现结构体（PIMPL 模式）
         *
         * 用于隐藏 libevent 相关结构体和实现细节：
         * - event_base* base;          // libevent 事件循环
         * - evconnlistener* listener;  // TCP 监听器
         * - OnDataCallback callback;    // 收到数据时的业务回调
         * - 客户端连接管理：fd -> bufferevent 映射
         *   - 每个客户端对应一个 bufferevent
         *   - bufferevent 内部维护输入/输出缓冲区
         *   - 输入缓冲区缓存接收数据，回调处理
         *   - 输出缓冲区缓存待发送数据，异步发送
         */
        Impl *impl_;
    };

} // namespace Net

#endif // NETWORK_MODULE_H
