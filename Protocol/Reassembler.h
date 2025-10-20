#pragma

#include "ProtocolModule.h"
#include <vector>

namespace Protocol
{
    /**
     * @brief 单用户单消息分片组装器
     * 用于将拆分的分片重新组装为完整消息
     */

    class Reassembler
    {
    public:
        Reassembler();
        /**
         * @brief 追加分片
         * @param header 分片头
         * @param data 分片数据
         * @return true 表示整条消息已经组装完成
         */
        bool append(const ProtocolHeader &header, const char *data);
        /**
         * @brief 获取组装完成的完整数据
         * @return 完整消息数据
         */
        std::vector<char> getData() const;
        /**
         * @brief 重置组装void器，准备组装下一条消息
         */
        void reset();

    private:
        uint32_t expectedSeq;     ///< 当前期望的分片序号
        uint32_t totalSeq;        ///< 总分片数
        uint32_t totalLength;     ///< 总长度
        std::vector<char> buffer; ///< 数据缓冲区
    };

}