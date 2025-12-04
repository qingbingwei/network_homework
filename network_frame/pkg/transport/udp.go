package transport

import (
	"encoding/binary"
	"errors"
	"fmt"
)

const (
	// UDPHeaderSize UDP首部长度为8字节
	UDPHeaderSize = 8
)

// UDPHeader UDP首部结构
type UDPHeader struct {
	SrcPort  uint16 // 源端口
	DstPort  uint16 // 目的端口
	Length   uint16 // UDP长度（首部+数据）
	Checksum uint16 // 校验和（可选，置0）
}

// UDPDatagram UDP数据报
type UDPDatagram struct {
	Header  UDPHeader
	Payload []byte
}

// NewUDPDatagram 创建新的UDP数据报
func NewUDPDatagram(srcPort, dstPort uint16, payload []byte) *UDPDatagram {
	return &UDPDatagram{
		Header: UDPHeader{
			SrcPort:  srcPort,
			DstPort:  dstPort,
			Length:   uint16(UDPHeaderSize + len(payload)),
			Checksum: 0,
		},
		Payload: payload,
	}
}

// Encode 将UDP数据报编码为字节切片
func (u *UDPDatagram) Encode() []byte {
	buf := make([]byte, UDPHeaderSize+len(u.Payload))

	binary.BigEndian.PutUint16(buf[0:2], u.Header.SrcPort)
	binary.BigEndian.PutUint16(buf[2:4], u.Header.DstPort)
	binary.BigEndian.PutUint16(buf[4:6], u.Header.Length)
	binary.BigEndian.PutUint16(buf[6:8], u.Header.Checksum)

	copy(buf[UDPHeaderSize:], u.Payload)

	return buf
}

// DecodeUDP 从字节切片解码UDP数据报
func DecodeUDP(data []byte) (*UDPDatagram, error) {
	if len(data) < UDPHeaderSize {
		return nil, errors.New("data too short for UDP header")
	}

	header := UDPHeader{
		SrcPort:  binary.BigEndian.Uint16(data[0:2]),
		DstPort:  binary.BigEndian.Uint16(data[2:4]),
		Length:   binary.BigEndian.Uint16(data[4:6]),
		Checksum: binary.BigEndian.Uint16(data[6:8]),
	}

	payloadLen := int(header.Length) - UDPHeaderSize
	if payloadLen < 0 || len(data) < UDPHeaderSize+payloadLen {
		return nil, errors.New("invalid UDP length field")
	}

	payload := make([]byte, payloadLen)
	copy(payload, data[UDPHeaderSize:UDPHeaderSize+payloadLen])

	return &UDPDatagram{
		Header:  header,
		Payload: payload,
	}, nil
}

// String 返回UDP数据报的字符串表示
func (u *UDPDatagram) String() string {
	return fmt.Sprintf("UDP Datagram:\n"+
		"  Source Port: %d\n"+
		"  Dest Port: %d\n"+
		"  Length: %d\n"+
		"  Checksum: 0x%04X\n"+
		"  Payload Size: %d bytes",
		u.Header.SrcPort,
		u.Header.DstPort,
		u.Header.Length,
		u.Header.Checksum,
		len(u.Payload))
}

// GetPayload 获取UDP数据报的有效载荷
func (u *UDPDatagram) GetPayload() []byte {
	return u.Payload
}
