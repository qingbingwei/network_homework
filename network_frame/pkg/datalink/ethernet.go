package datalink

import (
	"encoding/binary"
	"errors"
	"fmt"
	"net"
)

const (
	// EthernetHeaderSize 以太网首部长度为14字节
	EthernetHeaderSize = 14
	// MACAddressSize MAC地址长度为6字节
	MACAddressSize = 6
	// EtherTypeIPv4 IPv4的以太网类型字段
	EtherTypeIPv4 = 0x0800
)

// EthernetHeader 以太网首部结构
type EthernetHeader struct {
	DstMAC    net.HardwareAddr
	SrcMAC    net.HardwareAddr
	EtherType uint16
}

// EthernetFrame 以太网帧
type EthernetFrame struct {
	Header  EthernetHeader
	Payload []byte
}

// NewEthernetFrame 创建新的以太网帧
func NewEthernetFrame(srcMAC, dstMAC net.HardwareAddr, etherType uint16, payload []byte) *EthernetFrame {
	return &EthernetFrame{
		Header: EthernetHeader{
			DstMAC:    dstMAC,
			SrcMAC:    srcMAC,
			EtherType: etherType,
		},
		Payload: payload,
	}
}

// NewEthernetFrameIPv4 创建包含IPv4数据的以太网帧
func NewEthernetFrameIPv4(srcMAC, dstMAC net.HardwareAddr, payload []byte) *EthernetFrame {
	return NewEthernetFrame(srcMAC, dstMAC, EtherTypeIPv4, payload)
}

// Encode 将以太网帧编码为字节切片
func (f *EthernetFrame) Encode() []byte {
	buf := make([]byte, EthernetHeaderSize+len(f.Payload))

	copy(buf[0:6], f.Header.DstMAC)
	copy(buf[6:12], f.Header.SrcMAC)
	binary.BigEndian.PutUint16(buf[12:14], f.Header.EtherType)
	copy(buf[EthernetHeaderSize:], f.Payload)

	return buf
}

// DecodeEthernet 从字节切片解码以太网帧
func DecodeEthernet(data []byte) (*EthernetFrame, error) {
	if len(data) < EthernetHeaderSize {
		return nil, errors.New("data too short for Ethernet header")
	}

	dstMAC := make(net.HardwareAddr, MACAddressSize)
	srcMAC := make(net.HardwareAddr, MACAddressSize)
	copy(dstMAC, data[0:6])
	copy(srcMAC, data[6:12])

	header := EthernetHeader{
		DstMAC:    dstMAC,
		SrcMAC:    srcMAC,
		EtherType: binary.BigEndian.Uint16(data[12:14]),
	}

	payload := make([]byte, len(data)-EthernetHeaderSize)
	copy(payload, data[EthernetHeaderSize:])

	return &EthernetFrame{
		Header:  header,
		Payload: payload,
	}, nil
}

// String 返回以太网帧的字符串表示
func (f *EthernetFrame) String() string {
	etherTypeName := "Unknown"
	if f.Header.EtherType == EtherTypeIPv4 {
		etherTypeName = "IPv4"
	}

	return fmt.Sprintf("Ethernet Frame:\n"+
		"  Dest MAC: %s\n"+
		"  Source MAC: %s\n"+
		"  EtherType: 0x%04X (%s)\n"+
		"  Payload Size: %d bytes",
		f.Header.DstMAC,
		f.Header.SrcMAC,
		f.Header.EtherType,
		etherTypeName,
		len(f.Payload))
}

// GetPayload 获取以太网帧的有效载荷
func (f *EthernetFrame) GetPayload() []byte {
	return f.Payload
}

// IsIPv4 检查类型是否为IPv4
func (f *EthernetFrame) IsIPv4() bool {
	return f.Header.EtherType == EtherTypeIPv4
}

// ParseMAC 解析MAC地址字符串
func ParseMAC(mac string) (net.HardwareAddr, error) {
	return net.ParseMAC(mac)
}
