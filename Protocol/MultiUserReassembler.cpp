#include "MultiUserReassembler.h"

namespace Protocol
{
    bool MultiUserReassembler::append(const ProtocolHeader &header, const char *data, std::vector<char> &outData)
    {
        auto &userEntry = table[header.userId];
        std::lock_guard<std::mutex> lock(userEntry.mtx);

        auto &reassembler = userEntry.msgs[header.msgId];
        bool complete = reassembler.append(header, data);

        if (complete)
        {
            outData = reassembler.getData();
            userEntry.msgs.erase(header.msgId);
            if (userEntry.msgs.empty())
            {
                table.erase(header.userId);
            }
            reassembler.reset();
        }
        return complete;
    }
} // namespace Protocol
