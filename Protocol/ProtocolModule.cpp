#include "ProtocolModule.h"
#include <algorithm>

namespace Protocol
{
    std::vector<Message> splitMessage(uint32_t userId, uint64_t msgId, MsgType type, const char *data, size_t len)
    {
        std::vector<Message> chunks;
        size_t total_seq = (len + MAX_CHUNK_SIZE - 1) / MAX_CHUNK_SIZE;

        for (size_t i = 0; i < total_seq; i++)
        {
            struct Message msg;
            size_t offset = i * MAX_CHUNK_SIZE;

            size_t chunkLen = std::min(len - offset, MAX_CHUNK_SIZE);

            msg.header.userId = userId;
            msg.header.msgId = msgId;
            msg.header.type = static_cast<uint32_t>(type);
            msg.header.totalLength = static_cast<uint32_t>(len);
            msg.header.seq = static_cast<uint32_t>(i);
            msg.header.totalSeq = static_cast<uint32_t>(total_seq);
            msg.header.payloadLen = static_cast<uint32_t>(chunkLen);

            msg.payload.resize(chunkLen);
            std::memcmp(msg.payload.data(), data + offset, chunkLen);
            chunks.push_back(std::move(msg));
        }
    }

    std::vector<char> serializeMessage(const Message &msg)
    {
        std::vector<char> buf(sizeof(ProtocolHeader) + msg.payload.size());
        std::memcpy(buf.data(), &msg.header, sizeof(ProtocolHeader));
        std::memcpy(buf.data() + sizeof(ProtocolHeader), msg.payload.data(), msg.payload.size());
        return buf;
    }
}