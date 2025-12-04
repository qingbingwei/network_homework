#include "receiver/receiver.h"
#include <fstream>
#include <iostream>
#include <iomanip>

namespace receiver {

ParsedPacket Receiver::decapsulate(const std::vector<uint8_t>& data) {
    ParsedPacket result;
    
    std::cout << "=== Starting Decapsulation ===" << std::endl;
    std::cout << "Total bytes received: " << data.size() << std::endl << std::endl;
    
    // 1. 解析以太网帧
    std::cout << "--- Data Link Layer (Ethernet II) ---" << std::endl;
    auto ethFrame = datalink::EthernetFrame::decode(data);
    
    std::cout << "  Dest MAC: " << datalink::EthernetFrame::macToString(ethFrame.getHeader().dstMAC) << std::endl;
    std::cout << "  Source MAC: " << datalink::EthernetFrame::macToString(ethFrame.getHeader().srcMAC) << std::endl;
    std::cout << "  EtherType: 0x" << std::hex << std::setfill('0') << std::setw(4) 
              << ethFrame.getHeader().etherType << std::dec << std::endl;
    
    if (!ethFrame.isIPv4()) {
        throw std::runtime_error("EtherType is not IPv4 (0x0800)");
    }
    std::cout << "  EtherType verified: IPv4" << std::endl;
    
    result.ethernetFrame = std::make_unique<datalink::EthernetFrame>(ethFrame);
    
    // 2. 解析IPv4数据报
    std::cout << "\n--- Network Layer (IPv4) ---" << std::endl;
    auto ipPacket = network::IPv4Packet::decode(ethFrame.getPayload());
    
    auto ipHeader = ipPacket.getHeader();
    std::cout << "  Version: " << static_cast<int>(ipHeader.version) << std::endl;
    std::cout << "  Header Length: " << (ipHeader.ihl * 4) << " bytes" << std::endl;
    std::cout << "  Total Length: " << ipHeader.totalLength << " bytes" << std::endl;
    std::cout << "  TTL: " << static_cast<int>(ipHeader.ttl) << std::endl;
    std::cout << "  Protocol: " << static_cast<int>(ipHeader.protocol) << std::endl;
    std::cout << "  Source IP: " << network::IPv4Packet::ipToString(ipHeader.srcIP) << std::endl;
    std::cout << "  Dest IP: " << network::IPv4Packet::ipToString(ipHeader.dstIP) << std::endl;
    
    if (!ipPacket.isUDP()) {
        throw std::runtime_error("Protocol is not UDP (17)");
    }
    std::cout << "  Protocol verified: UDP (17)" << std::endl;
    
    result.ipv4Packet = std::make_unique<network::IPv4Packet>(ipPacket);
    
    // 3. 解析UDP数据报
    std::cout << "\n--- Transport Layer (UDP) ---" << std::endl;
    auto udpDatagram = transport::UDPDatagram::decode(ipPacket.getPayload());
    
    auto udpHeader = udpDatagram.getHeader();
    std::cout << "  Source Port: " << udpHeader.srcPort << std::endl;
    std::cout << "  Dest Port: " << udpHeader.dstPort << std::endl;
    std::cout << "  UDP Length: " << udpHeader.length << " bytes" << std::endl;
    std::cout << "  Checksum: 0x" << std::hex << std::setfill('0') << std::setw(4) 
              << udpHeader.checksum << std::dec << std::endl;
    
    result.udpDatagram = std::make_unique<transport::UDPDatagram>(udpDatagram);
    
    // 4. 还原应用层数据
    std::cout << "\n--- Application Layer ---" << std::endl;
    auto appData = application::Data(udpDatagram.getPayload());
    
    std::cout << "  Data: " << appData.getPayloadString() << std::endl;
    std::cout << "  Size: " << appData.size() << " bytes" << std::endl;
    
    result.applicationData = std::make_unique<application::Data>(appData);
    
    std::cout << "\n=== Decapsulation Complete ===" << std::endl;
    
    return result;
}

ParsedPacket Receiver::decapsulateFromFile(const std::string& filename) {
    std::cout << "Reading file: " << filename << std::endl << std::endl;
    
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }
    
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
    file.close();
    
    return decapsulate(data);
}

std::string Receiver::getApplicationData(const std::string& filename) {
    auto parsed = decapsulateFromFile(filename);
    return parsed.applicationData->getPayloadString();
}

} // namespace receiver
