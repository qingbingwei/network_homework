#ifndef SENDER_H
#define SENDER_H

#include <cstdint>
#include <vector>
#include <string>
#include "application/application.h"
#include "network/ipv4.h"
#include "datalink/ethernet.h"

namespace sender {

// 发送配置
struct Config {
    uint16_t srcPort;
    uint16_t dstPort;
    network::IPv4Address srcIP;
    network::IPv4Address dstIP;
    datalink::MACAddress srcMAC;
    datalink::MACAddress dstMAC;
};

// 封装模块
class Sender {
public:
    explicit Sender(const Config& config);
    
    // 封装数据，返回完整的以太网帧字节
    std::vector<uint8_t> encapsulate(const application::Data& data);
    
    // 封装数据并保存到文件
    void encapsulateAndSave(const application::Data& data, const std::string& filename);
    
    // 获取默认配置
    static Config defaultConfig();

private:
    Config config_;
};

} // namespace sender

#endif // SENDER_H
