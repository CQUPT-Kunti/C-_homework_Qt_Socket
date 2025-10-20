#pragma
#include "Reassembler.h"
#include "ProtocolModule.h"

#include <map>
#include <mutex>
#include <vector>

namespace Protocol
{
    /**
     * @brief 多用户多消息分片组装器
     * 使用细粒度锁，每个用户独立锁，支持高并发
     */
    class MultiUserReassembler
    {
    private:
        struct UserEntry
        {
            std::mutex mtx;                       ///< 用户独立锁
            std::map<uint64_t, Reassembler> msgs; ///< 该用户的所有消息
        };

        std::map<uint32_t, UserEntry> table; ///< userId -> UserEntry
    public:
        /**
         * @brief 追加分片
         * @param header 分片头
         * @param data 分片数据
         * @param outData 如果返回 true，outData 为完整消息
         * @return true 表示该消息已组装完成
         */
        bool append(const ProtocolHeader &header, const char *data, std::vector<char> &outData);
    };
}