#include "network/ipv4.h"
#include <sstream>
#include <iomanip>
#include <cstring>

namespace network {

IPv4Packet::IPv4Packet(const IPv4Address& srcIP, const IPv4Address& dstIP,
                       uint8_t protocol, const std::vector<uint8_t>& payload)
    : payload_(payload) {
    header_.version = 4;
    header_.ihl = 5;  // 20字节 / 4 = 5
    header_.tos = 0;
    header_.totalLength = static_cast<uint16_t>(IPV4_HEADER_SIZE + payload.size());
    header_.identification = 0;
    header_.flags = 0;
    header_.fragmentOffset = 0;
    header_.ttl = DEFAULT_TTL;
    header_.protocol = protocol;
    header_.headerChecksum = 0;
    header_.srcIP = srcIP;
    header_.dstIP = dstIP;
}

IPv4Packet IPv4Packet::createUDP(const IPv4Address& srcIP, const IPv4Address& dstIP,
                                   const std::vector<uint8_t>& payload) {
    return IPv4Packet(srcIP, dstIP, PROTOCOL_UDP, payload);
}

std::vector<uint8_t> IPv4Packet::encode() const {
    std::vector<uint8_t> buf(IPV4_HEADER_SIZE + payload_.size());
    
    // 版本和首部长度
    buf[0] = (header_.version << 4) | (header_.ihl & 0x0F);
    // TOS
    buf[1] = header_.tos;
    // 总长度
    buf[2] = (header_.totalLength >> 8) & 0xFF;
    buf[3] = header_.totalLength & 0xFF;
    // 标识
    buf[4] = (header_.identification >> 8) & 0xFF;
    buf[5] = header_.identification & 0xFF;
    // 标志和片偏移
    uint16_t flagsFragOffset = (static_cast<uint16_t>(header_.flags) << 13) | 
                                (header_.fragmentOffset & 0x1FFF);
    buf[6] = (flagsFragOffset >> 8) & 0xFF;
    buf[7] = flagsFragOffset & 0xFF;
    // TTL
    buf[8] = header_.ttl;
    // 协议
    buf[9] = header_.protocol;
    // 首部校验和（先置0）
    buf[10] = 0;
    buf[11] = 0;
    // 源IP地址
    std::copy(header_.srcIP.begin(), header_.srcIP.end(), buf.begin() + 12);
    // 目的IP地址
    std::copy(header_.dstIP.begin(), header_.dstIP.end(), buf.begin() + 16);
    
    // 计算首部校验和
    std::vector<uint8_t> headerBuf(buf.begin(), buf.begin() + IPV4_HEADER_SIZE);
    uint16_t checksum = calculateChecksum(headerBuf);
    buf[10] = (checksum >> 8) & 0xFF;
    buf[11] = checksum & 0xFF;
    
    // 写入数据
    std::copy(payload_.begin(), payload_.end(), buf.begin() + IPV4_HEADER_SIZE);
    
    return buf;
}

uint16_t IPv4Packet::calculateChecksum(const std::vector<uint8_t>& header) {
    uint32_t sum = 0;
    for (size_t i = 0; i < header.size(); i += 2) {
        uint16_t word = (static_cast<uint16_t>(header[i]) << 8);
        if (i + 1 < header.size()) {
            word |= header[i + 1];
        }
        sum += word;
    }
    while (sum > 0xFFFF) {
        sum = (sum >> 16) + (sum & 0xFFFF);
    }
    return ~static_cast<uint16_t>(sum);
}

IPv4Packet IPv4Packet::decode(const std::vector<uint8_t>& data) {
    if (data.size() < IPV4_HEADER_SIZE) {
        throw std::runtime_error("Data too short for IPv4 header");
    }
    
    uint8_t version = (data[0] >> 4) & 0x0F;
    if (version != 4) {
        throw std::runtime_error("Invalid IP version: " + std::to_string(version));
    }
    
    uint8_t ihl = data[0] & 0x0F;
    size_t headerLen = static_cast<size_t>(ihl) * 4;
    if (headerLen < IPV4_HEADER_SIZE || data.size() < headerLen) {
        throw std::runtime_error("Invalid IP header length");
    }
    
    uint16_t totalLength = (static_cast<uint16_t>(data[2]) << 8) | data[3];
    if (totalLength > data.size()) {
        throw std::runtime_error("Invalid total length");
    }
    
    uint16_t flagsFragOffset = (static_cast<uint16_t>(data[6]) << 8) | data[7];
    
    IPv4Packet packet;
    packet.header_.version = version;
    packet.header_.ihl = ihl;
    packet.header_.tos = data[1];
    packet.header_.totalLength = totalLength;
    packet.header_.identification = (static_cast<uint16_t>(data[4]) << 8) | data[5];
    packet.header_.flags = static_cast<uint8_t>(flagsFragOffset >> 13);
    packet.header_.fragmentOffset = flagsFragOffset & 0x1FFF;
    packet.header_.ttl = data[8];
    packet.header_.protocol = data[9];
    packet.header_.headerChecksum = (static_cast<uint16_t>(data[10]) << 8) | data[11];
    std::copy(data.begin() + 12, data.begin() + 16, packet.header_.srcIP.begin());
    std::copy(data.begin() + 16, data.begin() + 20, packet.header_.dstIP.begin());
    
    size_t payloadLen = totalLength - headerLen;
    packet.payload_.assign(data.begin() + headerLen, data.begin() + headerLen + payloadLen);
    
    return packet;
}

std::vector<uint8_t> IPv4Packet::getPayload() const {
    return payload_;
}

IPv4Header IPv4Packet::getHeader() const {
    return header_;
}

bool IPv4Packet::isUDP() const {
    return header_.protocol == PROTOCOL_UDP;
}

std::string IPv4Packet::ipToString(const IPv4Address& ip) {
    std::ostringstream oss;
    oss << static_cast<int>(ip[0]) << "."
        << static_cast<int>(ip[1]) << "."
        << static_cast<int>(ip[2]) << "."
        << static_cast<int>(ip[3]);
    return oss.str();
}

IPv4Address IPv4Packet::parseIP(const std::string& ipStr) {
    IPv4Address ip{};
    int parts[4];
    if (sscanf(ipStr.c_str(), "%d.%d.%d.%d", &parts[0], &parts[1], &parts[2], &parts[3]) != 4) {
        throw std::runtime_error("Invalid IP address format: " + ipStr);
    }
    for (int i = 0; i < 4; ++i) {
        if (parts[i] < 0 || parts[i] > 255) {
            throw std::runtime_error("Invalid IP address octet: " + ipStr);
        }
        ip[i] = static_cast<uint8_t>(parts[i]);
    }
    return ip;
}

std::string IPv4Packet::toString() const {
    std::ostringstream oss;
    oss << "IPv4 Packet:\n"
        << "  Version: " << static_cast<int>(header_.version) << "\n"
        << "  IHL: " << static_cast<int>(header_.ihl) << " (" << (header_.ihl * 4) << " bytes)\n"
        << "  Total Length: " << header_.totalLength << "\n"
        << "  TTL: " << static_cast<int>(header_.ttl) << "\n"
        << "  Protocol: " << static_cast<int>(header_.protocol) << "\n"
        << "  Header Checksum: 0x" << std::hex << std::setfill('0') << std::setw(4) << header_.headerChecksum << "\n"
        << "  Source IP: " << ipToString(header_.srcIP) << "\n"
        << "  Dest IP: " << ipToString(header_.dstIP) << "\n"
        << "  Payload Size: " << std::dec << payload_.size() << " bytes";
    return oss.str();
}

} // namespace network
