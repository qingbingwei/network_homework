#ifndef UDP_H
#define UDP_H

#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>

namespace transport {

constexpr size_t UDP_HEADER_SIZE = 8;

// UDP首部结构
struct UDPHeader {
    uint16_t srcPort;   // 源端口
    uint16_t dstPort;   // 目的端口
    uint16_t length;    // UDP长度（首部+数据）
    uint16_t checksum;  // 校验和（可选，置0）
};

// UDP数据报类
class UDPDatagram {
public:
    UDPDatagram() = default;
    UDPDatagram(uint16_t srcPort, uint16_t dstPort, const std::vector<uint8_t>& payload);

    // 编码为字节数组
    std::vector<uint8_t> encode() const;
    
    // 从字节数组解码
    static UDPDatagram decode(const std::vector<uint8_t>& data);
    
    // 获取有效载荷
    std::vector<uint8_t> getPayload() const;
    
    // 获取首部信息
    UDPHeader getHeader() const;
    
    // 字符串表示
    std::string toString() const;

private:
    UDPHeader header_{};
    std::vector<uint8_t> payload_;
};

} // namespace transport

#endif // UDP_H
