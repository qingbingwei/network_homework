#ifndef RECEIVER_H
#define RECEIVER_H

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include "application/application.h"
#include "transport/udp.h"
#include "network/ipv4.h"
#include "datalink/ethernet.h"

namespace receiver {

// 解析后的数据包结构
struct ParsedPacket {
    std::unique_ptr<datalink::EthernetFrame> ethernetFrame;
    std::unique_ptr<network::IPv4Packet> ipv4Packet;
    std::unique_ptr<transport::UDPDatagram> udpDatagram;
    std::unique_ptr<application::Data> applicationData;
};

// 解封装模块
class Receiver {
public:
    Receiver() = default;
    
    // 解封装数据
    ParsedPacket decapsulate(const std::vector<uint8_t>& data);
    
    // 从文件读取并解封装数据
    ParsedPacket decapsulateFromFile(const std::string& filename);
    
    // 从文件中提取应用层数据字符串
    std::string getApplicationData(const std::string& filename);
};

} // namespace receiver

#endif // RECEIVER_H
