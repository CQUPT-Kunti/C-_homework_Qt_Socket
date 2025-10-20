#pragma once

#include <cstdint>
#include <vector>
#include <cstring>
#include <map>
namespace Protocol
{
    // 协议类型
    enum MsgType : uint32_t
    {
        TEXT = 0,
        IMAGE = 1,
        FILE = 2,
        SCREEN = 3
    };

    // 最大单次分片长度
    constexpr size_t MAX_CHUNK_SIZE = 64 * 1024; // 64KB

#pragma pack(push, 1)
    struct ProtocolHeader
    {
        uint32_t userId; // 发送用户ID
        uint64_t msgId;  // 消息唯一ID
        uint32_t type;
        uint32_t totalLength; // 消息总长度
        uint32_t seq;         // 当前分片
        uint32_t totalSeq;    // 分片总数
        uint32_t payloadLen;  // 当前分片长度
    };
#pragma pack(pop)

    struct Message
    {
        ProtocolHeader header;
        std::vector<char> payload;
    };

    // 协议模块接口
    class ProtocolMessage
    {
    public:
        /**
         * @brief 将完整消息拆分成多个分片
         * @param userId 发送用户ID
         * @param msgId 消息唯一ID
         * @param type 消息类型
         * @param data 消息内容指针
         * @param len 消息长度
         * @return 拆分后的分片列表
         */
        static std::vector<Message> splitMessage(uint32_t userId, uint64_t msgId, MsgType type, const char *data, size_t len);
        /**
         * @brief 将单个分片序列化为字节流，可直接发送
         * @param msg 分片消息
         * @return 序列化后的字节流
         */
        static std::vector<char> serializeMessage(const Message &msg);
    };

};