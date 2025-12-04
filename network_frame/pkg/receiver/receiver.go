package receiver

import (
	"errors"
	"fmt"
	"os"

	"network_frame/pkg/application"
	"network_frame/pkg/datalink"
	"network_frame/pkg/network"
	"network_frame/pkg/transport"
)

// ParsedPacket 解析后的数据包结构
type ParsedPacket struct {
	EthernetFrame   *datalink.EthernetFrame
	IPv4Packet      *network.IPv4Packet
	UDPDatagram     *transport.UDPDatagram
	ApplicationData *application.Data
}

// Receiver 解封装模块
type Receiver struct{}

// NewReceiver 创建新的接收器
func NewReceiver() *Receiver {
	return &Receiver{}
}

// Decapsulate 解封装数据
func (r *Receiver) Decapsulate(data []byte) (*ParsedPacket, error) {
	result := &ParsedPacket{}

	fmt.Println("=== Starting Decapsulation ===")
	fmt.Printf("Total bytes received: %d\n\n", len(data))

	// 解析以太网帧
	fmt.Println("--- Data Link Layer (Ethernet II) ---")
	ethFrame, err := datalink.DecodeEthernet(data)
	if err != nil {
		return nil, fmt.Errorf("failed to decode Ethernet frame: %w", err)
	}
	result.EthernetFrame = ethFrame

	fmt.Printf("  Dest MAC: %s\n", ethFrame.Header.DstMAC)
	fmt.Printf("  Source MAC: %s\n", ethFrame.Header.SrcMAC)
	fmt.Printf("  EtherType: 0x%04X\n", ethFrame.Header.EtherType)

	if !ethFrame.IsIPv4() {
		return nil, errors.New("EtherType is not IPv4 (0x0800)")
	}
	fmt.Println("  EtherType verified: IPv4")

	// 解析IPv4数据报
	fmt.Println("\n--- Network Layer (IPv4) ---")
	ipPacket, err := network.DecodeIPv4(ethFrame.GetPayload())
	if err != nil {
		return nil, fmt.Errorf("failed to decode IPv4 packet: %w", err)
	}
	result.IPv4Packet = ipPacket

	fmt.Printf("  Version: %d\n", ipPacket.Header.Version)
	fmt.Printf("  Header Length: %d bytes\n", ipPacket.Header.IHL*4)
	fmt.Printf("  Total Length: %d bytes\n", ipPacket.Header.TotalLength)
	fmt.Printf("  TTL: %d\n", ipPacket.Header.TTL)
	fmt.Printf("  Protocol: %d\n", ipPacket.Header.Protocol)
	fmt.Printf("  Source IP: %s\n", ipPacket.Header.SrcIP)
	fmt.Printf("  Dest IP: %s\n", ipPacket.Header.DstIP)

	if !ipPacket.IsUDP() {
		return nil, fmt.Errorf("protocol is not UDP (17), got: %d", ipPacket.Header.Protocol)
	}
	fmt.Println("  Protocol verified: UDP (17)")

	// 解析UDP数据报
	fmt.Println("\n--- Transport Layer (UDP) ---")
	udpDatagram, err := transport.DecodeUDP(ipPacket.GetPayload())
	if err != nil {
		return nil, fmt.Errorf("failed to decode UDP datagram: %w", err)
	}
	result.UDPDatagram = udpDatagram

	fmt.Printf("  Source Port: %d\n", udpDatagram.Header.SrcPort)
	fmt.Printf("  Dest Port: %d\n", udpDatagram.Header.DstPort)
	fmt.Printf("  UDP Length: %d bytes\n", udpDatagram.Header.Length)
	fmt.Printf("  Checksum: 0x%04X\n", udpDatagram.Header.Checksum)

	// 还原应用层数据
	fmt.Println("\n--- Application Layer ---")
	appData := application.NewDataFromBytes(udpDatagram.GetPayload())
	result.ApplicationData = appData

	fmt.Printf("  Data: %s\n", appData.GetPayloadString())
	fmt.Printf("  Size: %d bytes\n", appData.Size())

	fmt.Println("\n=== Decapsulation Complete ===")

	return result, nil
}

// DecapsulateFromFile 从文件读取并解封装数据
func (r *Receiver) DecapsulateFromFile(filename string) (*ParsedPacket, error) {
	fmt.Printf("Reading file: %s\n\n", filename)

	data, err := os.ReadFile(filename)
	if err != nil {
		return nil, fmt.Errorf("failed to read file: %w", err)
	}

	return r.Decapsulate(data)
}

// GetApplicationData 从文件中提取应用层数据字符串
func (r *Receiver) GetApplicationData(filename string) (string, error) {
	parsed, err := r.DecapsulateFromFile(filename)
	if err != nil {
		return "", err
	}
	return parsed.ApplicationData.GetPayloadString(), nil
}
