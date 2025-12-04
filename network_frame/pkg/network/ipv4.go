package network

import (
	"encoding/binary"
	"errors"
	"fmt"
	"net"
)

const (
	// IPv4HeaderSize IPv4首部长度（无选项字段）为20字节
	IPv4HeaderSize = 20
	// ProtocolUDP UDP协议号
	ProtocolUDP = 17
	// DefaultTTL 默认TTL值
	DefaultTTL = 64
)

// IPv4Header IPv4首部结构
type IPv4Header struct {
	Version        uint8
	IHL            uint8
	TOS            uint8
	TotalLength    uint16
	Identification uint16
	Flags          uint8
	FragmentOffset uint16
	TTL            uint8
	Protocol       uint8
	HeaderChecksum uint16
	SrcIP          net.IP
	DstIP          net.IP
}

// IPv4Packet IPv4数据报
type IPv4Packet struct {
	Header  IPv4Header
	Payload []byte
}

// NewIPv4Packet 创建新的IPv4数据报
func NewIPv4Packet(srcIP, dstIP net.IP, protocol uint8, payload []byte) *IPv4Packet {
	return &IPv4Packet{
		Header: IPv4Header{
			Version:        4,
			IHL:            5,
			TOS:            0,
			TotalLength:    uint16(IPv4HeaderSize + len(payload)),
			Identification: 0,
			Flags:          0,
			FragmentOffset: 0,
			TTL:            DefaultTTL,
			Protocol:       protocol,
			HeaderChecksum: 0,
			SrcIP:          srcIP.To4(),
			DstIP:          dstIP.To4(),
		},
		Payload: payload,
	}
}

// NewIPv4PacketUDP 创建包含UDP数据的IPv4数据报
func NewIPv4PacketUDP(srcIP, dstIP net.IP, payload []byte) *IPv4Packet {
	return NewIPv4Packet(srcIP, dstIP, ProtocolUDP, payload)
}

// Encode 将IPv4数据报编码为字节切片
func (p *IPv4Packet) Encode() []byte {
	buf := make([]byte, IPv4HeaderSize+len(p.Payload))

	buf[0] = (p.Header.Version << 4) | (p.Header.IHL & 0x0F)
	buf[1] = p.Header.TOS
	binary.BigEndian.PutUint16(buf[2:4], p.Header.TotalLength)
	binary.BigEndian.PutUint16(buf[4:6], p.Header.Identification)
	flagsFragOffset := (uint16(p.Header.Flags) << 13) | (p.Header.FragmentOffset & 0x1FFF)
	binary.BigEndian.PutUint16(buf[6:8], flagsFragOffset)
	buf[8] = p.Header.TTL
	buf[9] = p.Header.Protocol
	binary.BigEndian.PutUint16(buf[10:12], 0)
	copy(buf[12:16], p.Header.SrcIP.To4())
	copy(buf[16:20], p.Header.DstIP.To4())

	checksum := calculateChecksum(buf[:IPv4HeaderSize])
	binary.BigEndian.PutUint16(buf[10:12], checksum)
	p.Header.HeaderChecksum = checksum

	copy(buf[IPv4HeaderSize:], p.Payload)

	return buf
}

// calculateChecksum 计算IP首部校验和
func calculateChecksum(header []byte) uint16 {
	var sum uint32
	for i := 0; i < len(header); i += 2 {
		sum += uint32(binary.BigEndian.Uint16(header[i : i+2]))
	}
	for sum > 0xFFFF {
		sum = (sum >> 16) + (sum & 0xFFFF)
	}
	return ^uint16(sum)
}

// DecodeIPv4 从字节切片解码IPv4数据报
func DecodeIPv4(data []byte) (*IPv4Packet, error) {
	if len(data) < IPv4HeaderSize {
		return nil, errors.New("data too short for IPv4 header")
	}

	version := (data[0] >> 4) & 0x0F
	if version != 4 {
		return nil, fmt.Errorf("invalid IP version: %d", version)
	}

	ihl := data[0] & 0x0F
	headerLen := int(ihl) * 4
	if headerLen < IPv4HeaderSize || len(data) < headerLen {
		return nil, errors.New("invalid IP header length")
	}

	totalLength := binary.BigEndian.Uint16(data[2:4])
	if int(totalLength) > len(data) {
		return nil, errors.New("invalid total length")
	}

	flagsFragOffset := binary.BigEndian.Uint16(data[6:8])

	header := IPv4Header{
		Version:        version,
		IHL:            ihl,
		TOS:            data[1],
		TotalLength:    totalLength,
		Identification: binary.BigEndian.Uint16(data[4:6]),
		Flags:          uint8(flagsFragOffset >> 13),
		FragmentOffset: flagsFragOffset & 0x1FFF,
		TTL:            data[8],
		Protocol:       data[9],
		HeaderChecksum: binary.BigEndian.Uint16(data[10:12]),
		SrcIP:          net.IP(data[12:16]),
		DstIP:          net.IP(data[16:20]),
	}

	payloadLen := int(totalLength) - headerLen
	payload := make([]byte, payloadLen)
	copy(payload, data[headerLen:headerLen+payloadLen])

	return &IPv4Packet{
		Header:  header,
		Payload: payload,
	}, nil
}

// String 返回IPv4数据报的字符串表示
func (p *IPv4Packet) String() string {
	return fmt.Sprintf("IPv4 Packet:\n"+
		"  Version: %d\n"+
		"  IHL: %d (%d bytes)\n"+
		"  Total Length: %d\n"+
		"  TTL: %d\n"+
		"  Protocol: %d\n"+
		"  Header Checksum: 0x%04X\n"+
		"  Source IP: %s\n"+
		"  Dest IP: %s\n"+
		"  Payload Size: %d bytes",
		p.Header.Version,
		p.Header.IHL,
		p.Header.IHL*4,
		p.Header.TotalLength,
		p.Header.TTL,
		p.Header.Protocol,
		p.Header.HeaderChecksum,
		p.Header.SrcIP,
		p.Header.DstIP,
		len(p.Payload))
}

// GetPayload 获取IPv4数据报的有效载荷
func (p *IPv4Packet) GetPayload() []byte {
	return p.Payload
}

// IsUDP 检查协议是否为UDP
func (p *IPv4Packet) IsUDP() bool {
	return p.Header.Protocol == ProtocolUDP
}
