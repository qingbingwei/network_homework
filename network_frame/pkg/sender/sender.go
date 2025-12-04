package sender

import (
	"fmt"
	"net"
	"os"

	"network_frame/pkg/application"
	"network_frame/pkg/datalink"
	"network_frame/pkg/network"
	"network_frame/pkg/transport"
)

// Config 发送配置
type Config struct {
	SrcPort uint16
	DstPort uint16
	SrcIP   net.IP
	DstIP   net.IP
	SrcMAC  net.HardwareAddr
	DstMAC  net.HardwareAddr
}

// Sender 封装模块
type Sender struct {
	config Config
}

// NewSender 创建新的发送器
func NewSender(config Config) *Sender {
	return &Sender{
		config: config,
	}
}

// Encapsulate 封装数据，返回完整的以太网帧字节
func (s *Sender) Encapsulate(data *application.Data) ([]byte, error) {
	payload := data.GetPayload()
	fmt.Printf("Application Layer - Data: %s (%d bytes)\n", string(payload), len(payload))

	// 传输层 - 构建UDP数据报
	udpDatagram := transport.NewUDPDatagram(s.config.SrcPort, s.config.DstPort, payload)
	udpBytes := udpDatagram.Encode()
	fmt.Printf("\nTransport Layer (UDP):\n")
	fmt.Printf("  Source Port: %d\n", s.config.SrcPort)
	fmt.Printf("  Dest Port: %d\n", s.config.DstPort)
	fmt.Printf("  UDP Length: %d bytes (header: 8 + data: %d)\n", len(udpBytes), len(payload))

	// 网络层 - 构建IPv4数据报
	ipPacket := network.NewIPv4PacketUDP(s.config.SrcIP, s.config.DstIP, udpBytes)
	ipBytes := ipPacket.Encode()
	fmt.Printf("\nNetwork Layer (IPv4):\n")
	fmt.Printf("  Source IP: %s\n", s.config.SrcIP)
	fmt.Printf("  Dest IP: %s\n", s.config.DstIP)
	fmt.Printf("  Protocol: UDP (17)\n")
	fmt.Printf("  Total Length: %d bytes (header: 20 + UDP: %d)\n", len(ipBytes), len(udpBytes))

	// 数据链路层 - 构建以太网帧
	ethFrame := datalink.NewEthernetFrameIPv4(s.config.SrcMAC, s.config.DstMAC, ipBytes)
	ethBytes := ethFrame.Encode()
	fmt.Printf("\nData Link Layer (Ethernet II):\n")
	fmt.Printf("  Source MAC: %s\n", s.config.SrcMAC)
	fmt.Printf("  Dest MAC: %s\n", s.config.DstMAC)
	fmt.Printf("  EtherType: 0x0800 (IPv4)\n")
	fmt.Printf("  Frame Size: %d bytes (header: 14 + IP: %d)\n", len(ethBytes), len(ipBytes))

	return ethBytes, nil
}

// EncapsulateAndSave 封装数据并保存到文件
func (s *Sender) EncapsulateAndSave(data *application.Data, filename string) error {
	frameBytes, err := s.Encapsulate(data)
	if err != nil {
		return fmt.Errorf("encapsulation failed: %w", err)
	}

	err = os.WriteFile(filename, frameBytes, 0644)
	if err != nil {
		return fmt.Errorf("failed to write file: %w", err)
	}

	fmt.Printf("\nPhysical Layer:\n")
	fmt.Printf("  Saved to file: %s\n", filename)
	fmt.Printf("  Total bytes written: %d\n", len(frameBytes))

	return nil
}

// DefaultConfig 返回默认配置
func DefaultConfig() Config {
	srcMAC, _ := net.ParseMAC("00:11:22:33:44:55")
	dstMAC, _ := net.ParseMAC("66:77:88:99:AA:BB")

	return Config{
		SrcPort: 12345,
		DstPort: 80,
		SrcIP:   net.ParseIP("192.168.1.100"),
		DstIP:   net.ParseIP("192.168.1.1"),
		SrcMAC:  srcMAC,
		DstMAC:  dstMAC,
	}
}
