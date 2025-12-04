#include "transport/udp.h"
#include <sstream>
#include <iomanip>

namespace transport {

UDPDatagram::UDPDatagram(uint16_t srcPort, uint16_t dstPort, const std::vector<uint8_t>& payload)
    : payload_(payload) {
    header_.srcPort = srcPort;
    header_.dstPort = dstPort;
    header_.length = static_cast<uint16_t>(UDP_HEADER_SIZE + payload.size());
    header_.checksum = 0;  // 校验和置0
}

std::vector<uint8_t> UDPDatagram::encode() const {
    std::vector<uint8_t> buf(UDP_HEADER_SIZE + payload_.size());
    
    // 写入UDP首部（大端/网络字节序）
    buf[0] = (header_.srcPort >> 8) & 0xFF;
    buf[1] = header_.srcPort & 0xFF;
    buf[2] = (header_.dstPort >> 8) & 0xFF;
    buf[3] = header_.dstPort & 0xFF;
    buf[4] = (header_.length >> 8) & 0xFF;
    buf[5] = header_.length & 0xFF;
    buf[6] = (header_.checksum >> 8) & 0xFF;
    buf[7] = header_.checksum & 0xFF;
    
    // 写入数据
    std::copy(payload_.begin(), payload_.end(), buf.begin() + UDP_HEADER_SIZE);
    
    return buf;
}

UDPDatagram UDPDatagram::decode(const std::vector<uint8_t>& data) {
    if (data.size() < UDP_HEADER_SIZE) {
        throw std::runtime_error("Data too short for UDP header");
    }
    
    UDPDatagram datagram;
    
    // 解析首部
    datagram.header_.srcPort = (static_cast<uint16_t>(data[0]) << 8) | data[1];
    datagram.header_.dstPort = (static_cast<uint16_t>(data[2]) << 8) | data[3];
    datagram.header_.length = (static_cast<uint16_t>(data[4]) << 8) | data[5];
    datagram.header_.checksum = (static_cast<uint16_t>(data[6]) << 8) | data[7];
    
    size_t payloadLen = datagram.header_.length - UDP_HEADER_SIZE;
    if (data.size() < UDP_HEADER_SIZE + payloadLen) {
        throw std::runtime_error("Invalid UDP length field");
    }
    
    datagram.payload_.assign(data.begin() + UDP_HEADER_SIZE, 
                              data.begin() + UDP_HEADER_SIZE + payloadLen);
    
    return datagram;
}

std::vector<uint8_t> UDPDatagram::getPayload() const {
    return payload_;
}

UDPHeader UDPDatagram::getHeader() const {
    return header_;
}

std::string UDPDatagram::toString() const {
    std::ostringstream oss;
    oss << "UDP Datagram:\n"
        << "  Source Port: " << header_.srcPort << "\n"
        << "  Dest Port: " << header_.dstPort << "\n"
        << "  Length: " << header_.length << "\n"
        << "  Checksum: 0x" << std::hex << std::setfill('0') << std::setw(4) << header_.checksum << "\n"
        << "  Payload Size: " << std::dec << payload_.size() << " bytes";
    return oss.str();
}

} // namespace transport
