#include "datalink/ethernet.h"
#include <sstream>
#include <iomanip>
#include <cstdio>

namespace datalink {

EthernetFrame::EthernetFrame(const MACAddress& srcMAC, const MACAddress& dstMAC,
                              uint16_t etherType, const std::vector<uint8_t>& payload)
    : payload_(payload) {
    header_.dstMAC = dstMAC;
    header_.srcMAC = srcMAC;
    header_.etherType = etherType;
}

EthernetFrame EthernetFrame::createIPv4(const MACAddress& srcMAC, const MACAddress& dstMAC,
                                         const std::vector<uint8_t>& payload) {
    return EthernetFrame(srcMAC, dstMAC, ETHERTYPE_IPV4, payload);
}

std::vector<uint8_t> EthernetFrame::encode() const {
    std::vector<uint8_t> buf(ETHERNET_HEADER_SIZE + payload_.size());
    
    // 目的MAC地址
    std::copy(header_.dstMAC.begin(), header_.dstMAC.end(), buf.begin());
    // 源MAC地址
    std::copy(header_.srcMAC.begin(), header_.srcMAC.end(), buf.begin() + 6);
    // 类型字段
    buf[12] = (header_.etherType >> 8) & 0xFF;
    buf[13] = header_.etherType & 0xFF;
    // 数据
    std::copy(payload_.begin(), payload_.end(), buf.begin() + ETHERNET_HEADER_SIZE);
    
    return buf;
}

EthernetFrame EthernetFrame::decode(const std::vector<uint8_t>& data) {
    if (data.size() < ETHERNET_HEADER_SIZE) {
        throw std::runtime_error("Data too short for Ethernet header");
    }
    
    EthernetFrame frame;
    std::copy(data.begin(), data.begin() + 6, frame.header_.dstMAC.begin());
    std::copy(data.begin() + 6, data.begin() + 12, frame.header_.srcMAC.begin());
    frame.header_.etherType = (static_cast<uint16_t>(data[12]) << 8) | data[13];
    
    frame.payload_.assign(data.begin() + ETHERNET_HEADER_SIZE, data.end());
    
    return frame;
}

std::vector<uint8_t> EthernetFrame::getPayload() const {
    return payload_;
}

EthernetHeader EthernetFrame::getHeader() const {
    return header_;
}

bool EthernetFrame::isIPv4() const {
    return header_.etherType == ETHERTYPE_IPV4;
}

std::string EthernetFrame::macToString(const MACAddress& mac) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (size_t i = 0; i < mac.size(); ++i) {
        if (i > 0) oss << ":";
        oss << std::setw(2) << static_cast<int>(mac[i]);
    }
    return oss.str();
}

MACAddress EthernetFrame::parseMAC(const std::string& macStr) {
    MACAddress mac{};
    int parts[6];
    if (sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x",
               &parts[0], &parts[1], &parts[2], &parts[3], &parts[4], &parts[5]) != 6) {
        throw std::runtime_error("Invalid MAC address format: " + macStr);
    }
    for (int i = 0; i < 6; ++i) {
        mac[i] = static_cast<uint8_t>(parts[i]);
    }
    return mac;
}

std::string EthernetFrame::toString() const {
    std::string etherTypeName = "Unknown";
    if (header_.etherType == ETHERTYPE_IPV4) {
        etherTypeName = "IPv4";
    }
    
    std::ostringstream oss;
    oss << "Ethernet Frame:\n"
        << "  Dest MAC: " << macToString(header_.dstMAC) << "\n"
        << "  Source MAC: " << macToString(header_.srcMAC) << "\n"
        << "  EtherType: 0x" << std::hex << std::setfill('0') << std::setw(4) << header_.etherType 
        << " (" << etherTypeName << ")\n"
        << "  Payload Size: " << std::dec << payload_.size() << " bytes";
    return oss.str();
}

} // namespace datalink
