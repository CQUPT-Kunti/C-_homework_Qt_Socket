#include "Reassembler.h"
#include <iostream>
#include <cstring>

namespace Protocol
{

    Reassembler::Reassembler()
        : expectedSeq(0), totalSeq(0), totalLength(0) {}

    bool Reassembler::append(const ProtocolHeader &header, const char *data)
    {
        // 第一次到来时初始化
        if (buffer.empty())
        {
            totalSeq = header.totalSeq;
            totalLength = header.totalLength;
            buffer.resize(totalLength);
        }
        size_t offset = header.seq * MAX_CHUNK_SIZE;
        // 边界校验
        if (offset > buffer.size() || offset + header.payloadLen > buffer.size())
        {
            std::cout << "[ERROR] offset overflow! seq=" << header.seq
                      << " payloadLen=" << header.payloadLen
                      << " buffer.size=" << buffer.size() << std::endl;
            return false;
        }
        std::memcpy(buffer.data() + offset, data, header.payloadLen);
        receivedSeq.insert(header.seq);
        return receivedSeq.size() == totalSeq;
    }

    std::vector<char> Reassembler::getData() const
    {
        return buffer;
    }

    void Reassembler::reset()
    {
        buffer.clear();
        expectedSeq = 0;
        totalSeq = 0;
        totalLength = 0;
        receivedSeq.clear();
    }

} // namespace Protocol
