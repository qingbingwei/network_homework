#include "sender/sender.h"
#include "transport/udp.h"
#include <fstream>
#include <iostream>

namespace sender {

Sender::Sender(const Config& config) : config_(config) {}

std::vector<uint8_t> Sender::encapsulate(const application::Data& data) {
    auto payload = data.getPayload();
    std::cout << "Application Layer - Data: " << data.getPayloadString() 
              << " (" << payload.size() << " bytes)" << std::endl;
    
    // 传输层 - 构建UDP数据报
    transport::UDPDatagram udpDatagram(config_.srcPort, config_.dstPort, payload);
    auto udpBytes = udpDatagram.encode();
    std::cout << "\nTransport Layer (UDP):" << std::endl;
    std::cout << "  Source Port: " << config_.srcPort << std::endl;
    std::cout << "  Dest Port: " << config_.dstPort << std::endl;
    std::cout << "  UDP Length: " << udpBytes.size() << " bytes (header: 8 + data: " 
              << payload.size() << ")" << std::endl;
    
    // 网络层 - 构建IPv4数据报
    auto ipPacket = network::IPv4Packet::createUDP(config_.srcIP, config_.dstIP, udpBytes);
    auto ipBytes = ipPacket.encode();
    std::cout << "\nNetwork Layer (IPv4):" << std::endl;
    std::cout << "  Source IP: " << network::IPv4Packet::ipToString(config_.srcIP) << std::endl;
    std::cout << "  Dest IP: " << network::IPv4Packet::ipToString(config_.dstIP) << std::endl;
    std::cout << "  Protocol: UDP (17)" << std::endl;
    std::cout << "  Total Length: " << ipBytes.size() << " bytes (header: 20 + UDP: " 
              << udpBytes.size() << ")" << std::endl;
    
    // 数据链路层 - 构建以太网帧
    auto ethFrame = datalink::EthernetFrame::createIPv4(config_.srcMAC, config_.dstMAC, ipBytes);
    auto ethBytes = ethFrame.encode();
    std::cout << "\nData Link Layer (Ethernet II):" << std::endl;
    std::cout << "  Source MAC: " << datalink::EthernetFrame::macToString(config_.srcMAC) << std::endl;
    std::cout << "  Dest MAC: " << datalink::EthernetFrame::macToString(config_.dstMAC) << std::endl;
    std::cout << "  EtherType: 0x0800 (IPv4)" << std::endl;
    std::cout << "  Frame Size: " << ethBytes.size() << " bytes (header: 14 + IP: " 
              << ipBytes.size() << ")" << std::endl;
    
    return ethBytes;
}

void Sender::encapsulateAndSave(const application::Data& data, const std::string& filename) {
    auto frameBytes = encapsulate(data);
    
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }
    
    file.write(reinterpret_cast<const char*>(frameBytes.data()), frameBytes.size());
    file.close();
    
    std::cout << "\nPhysical Layer:" << std::endl;
    std::cout << "  Saved to file: " << filename << std::endl;
    std::cout << "  Total bytes written: " << frameBytes.size() << std::endl;
}

Config Sender::defaultConfig() {
    Config config;
    config.srcPort = 12345;
    config.dstPort = 80;
    config.srcIP = network::IPv4Packet::parseIP("192.168.1.100");
    config.dstIP = network::IPv4Packet::parseIP("192.168.1.1");
    config.srcMAC = datalink::EthernetFrame::parseMAC("00:11:22:33:44:55");
    config.dstMAC = datalink::EthernetFrame::parseMAC("66:77:88:99:AA:BB");
    return config;
}

} // namespace sender
