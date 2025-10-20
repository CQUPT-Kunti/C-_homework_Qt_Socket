#include "Reassembler.h"

namespace Protocol
{
    Reassembler::Reassembler() : expectedSeq(0), totalSeq(0), totalLength(0) {}

    bool Reassembler::append(const ProtocolHeader &header, const char *data)
    {
        if (expectedSeq == 0)
        {
            totalSeq = header.totalSeq;
            totalLength = header.totalLength;
            buffer.resize(totalLength);
        }
        size_t offset = header.seq * MAX_CHUNK_SIZE;
        memcpy(buffer.data() + offset, data, header.payloadLen);
        expectedSeq++;
        return expectedSeq == totalSeq;
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
    }

}