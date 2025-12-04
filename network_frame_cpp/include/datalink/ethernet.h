#ifndef ETHERNET_H
#define ETHERNET_H

#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <stdexcept>

namespace datalink {

constexpr size_t ETHERNET_HEADER_SIZE = 14;
constexpr size_t MAC_ADDRESS_SIZE = 6;
constexpr uint16_t ETHERTYPE_IPV4 = 0x0800;

// MAC地址类型
using MACAddress = std::array<uint8_t, 6>;

// 以太网首部结构
struct EthernetHeader {
    MACAddress dstMAC;
    MACAddress srcMAC;
    uint16_t etherType;
};

// 以太网帧类
class EthernetFrame {
public:
    EthernetFrame() = default;
    EthernetFrame(const MACAddress& srcMAC, const MACAddress& dstMAC,
                  uint16_t etherType, const std::vector<uint8_t>& payload);
    
    // 创建IPv4以太网帧
    static EthernetFrame createIPv4(const MACAddress& srcMAC, const MACAddress& dstMAC,
                                     const std::vector<uint8_t>& payload);

    // 编码为字节数组
    std::vector<uint8_t> encode() const;
    
    // 从字节数组解码
    static EthernetFrame decode(const std::vector<uint8_t>& data);
    
    // 获取有效载荷
    std::vector<uint8_t> getPayload() const;
    
    // 获取首部信息
    EthernetHeader getHeader() const;
    
    // 检查是否为IPv4
    bool isIPv4() const;
    
    // 字符串表示
    std::string toString() const;
    
    // MAC地址转字符串
    static std::string macToString(const MACAddress& mac);
    
    // 字符串转MAC地址
    static MACAddress parseMAC(const std::string& macStr);

private:
    EthernetHeader header_{};
    std::vector<uint8_t> payload_;
};

} // namespace datalink

#endif // ETHERNET_H
