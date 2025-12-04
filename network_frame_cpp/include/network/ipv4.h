#ifndef IPV4_H
#define IPV4_H

#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <stdexcept>

namespace network {

constexpr size_t IPV4_HEADER_SIZE = 20;
constexpr uint8_t PROTOCOL_UDP = 17;
constexpr uint8_t DEFAULT_TTL = 64;

// IPv4地址类型
using IPv4Address = std::array<uint8_t, 4>;

// IPv4首部结构
struct IPv4Header {
    uint8_t version;
    uint8_t ihl;
    uint8_t tos;
    uint16_t totalLength;
    uint16_t identification;
    uint8_t flags;
    uint16_t fragmentOffset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t headerChecksum;
    IPv4Address srcIP;
    IPv4Address dstIP;
};

// IPv4数据报类
class IPv4Packet {
public:
    IPv4Packet() = default;
    IPv4Packet(const IPv4Address& srcIP, const IPv4Address& dstIP, 
               uint8_t protocol, const std::vector<uint8_t>& payload);
    
    // 创建UDP数据报
    static IPv4Packet createUDP(const IPv4Address& srcIP, const IPv4Address& dstIP,
                                 const std::vector<uint8_t>& payload);

    // 编码为字节数组
    std::vector<uint8_t> encode() const;
    
    // 从字节数组解码
    static IPv4Packet decode(const std::vector<uint8_t>& data);
    
    // 获取有效载荷
    std::vector<uint8_t> getPayload() const;
    
    // 获取首部信息
    IPv4Header getHeader() const;
    
    // 检查是否为UDP协议
    bool isUDP() const;
    
    // 字符串表示
    std::string toString() const;
    
    // IP地址转字符串
    static std::string ipToString(const IPv4Address& ip);
    
    // 字符串转IP地址
    static IPv4Address parseIP(const std::string& ipStr);

private:
    IPv4Header header_{};
    std::vector<uint8_t> payload_;
    
    // 计算首部校验和
    static uint16_t calculateChecksum(const std::vector<uint8_t>& header);
};

} // namespace network

#endif // IPV4_H
