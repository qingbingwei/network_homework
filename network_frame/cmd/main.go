package main

import (
	"fmt"
	"net"
	"os"

	"network_frame/pkg/application"
	"network_frame/pkg/receiver"
	"network_frame/pkg/sender"
)

const (
	defaultFilename = "packet.bin"
	defaultMessage  = "Hello Teacher"
)

func main() {
	if len(os.Args) < 2 {
		printUsage()
		return
	}

	switch os.Args[1] {
	case "send":
		runSender()
	case "receive":
		runReceiver()
	case "demo":
		runDemo()
	default:
		printUsage()
	}
}

func printUsage() {
	fmt.Println("Network Protocol Simulation Program")
	fmt.Println("====================================")
	fmt.Println("Usage:")
	fmt.Println("  go run cmd/main.go send    - Encapsulate data and save to packet.bin")
	fmt.Println("  go run cmd/main.go receive - Read packet.bin and decapsulate")
	fmt.Println("  go run cmd/main.go demo    - Full demo (encapsulate + decapsulate)")
}

func runSender() {
	fmt.Println("========================================")
	fmt.Println("       Encapsulation Module (Sender)")
	fmt.Println("========================================\n")

	message := defaultMessage
	if len(os.Args) > 2 {
		message = os.Args[2]
	}
	appData := application.NewData(message)

	config := sender.Config{
		SrcPort: 12345,
		DstPort: 80,
		SrcIP:   net.ParseIP("192.168.1.100"),
		DstIP:   net.ParseIP("192.168.1.1"),
		SrcMAC:  mustParseMAC("00:11:22:33:44:55"),
		DstMAC:  mustParseMAC("66:77:88:99:AA:BB"),
	}

	s := sender.NewSender(config)
	err := s.EncapsulateAndSave(appData, defaultFilename)
	if err != nil {
		fmt.Printf("Error: %v\n", err)
		os.Exit(1)
	}

	fmt.Println("\nEncapsulation complete!")
}

func runReceiver() {
	fmt.Println("========================================")
	fmt.Println("       Decapsulation Module (Receiver)")
	fmt.Println("========================================\n")

	filename := defaultFilename
	if len(os.Args) > 2 {
		filename = os.Args[2]
	}

	r := receiver.NewReceiver()
	parsed, err := r.DecapsulateFromFile(filename)
	if err != nil {
		fmt.Printf("Error: %v\n", err)
		os.Exit(1)
	}

	fmt.Println("\n========================================")
	fmt.Println("       Decapsulation Result Summary")
	fmt.Println("========================================")
	fmt.Printf("Original message: %s\n", parsed.ApplicationData.GetPayloadString())
}

func runDemo() {
	fmt.Println("========================================")
	fmt.Println("  Network Protocol Simulation - Demo")
	fmt.Println("========================================\n")

	message := defaultMessage
	if len(os.Args) > 2 {
		message = os.Args[2]
	}

	// Encapsulation
	fmt.Println("========================================")
	fmt.Println("         Part 1: Encapsulation")
	fmt.Println("========================================\n")

	appData := application.NewData(message)

	config := sender.Config{
		SrcPort: 12345,
		DstPort: 80,
		SrcIP:   net.ParseIP("192.168.1.100"),
		DstIP:   net.ParseIP("192.168.1.1"),
		SrcMAC:  mustParseMAC("00:11:22:33:44:55"),
		DstMAC:  mustParseMAC("66:77:88:99:AA:BB"),
	}

	s := sender.NewSender(config)
	err := s.EncapsulateAndSave(appData, defaultFilename)
	if err != nil {
		fmt.Printf("Error: %v\n", err)
		os.Exit(1)
	}

	// Decapsulation
	fmt.Println("\n\n========================================")
	fmt.Println("         Part 2: Decapsulation")
	fmt.Println("========================================\n")

	r := receiver.NewReceiver()
	parsed, err := r.DecapsulateFromFile(defaultFilename)
	if err != nil {
		fmt.Printf("Error: %v\n", err)
		os.Exit(1)
	}

	// Verification
	fmt.Println("\n========================================")
	fmt.Println("         Verification Result")
	fmt.Println("========================================")
	fmt.Printf("Original sent message: %s\n", message)
	fmt.Printf("Decapsulated message:  %s\n", parsed.ApplicationData.GetPayloadString())

	if message == parsed.ApplicationData.GetPayloadString() {
		fmt.Println("Verification SUCCESS: Messages match!")
	} else {
		fmt.Println("Verification FAILED: Messages don't match!")
	}
}

func mustParseMAC(mac string) net.HardwareAddr {
	addr, err := net.ParseMAC(mac)
	if err != nil {
		panic(fmt.Sprintf("invalid MAC address: %s", mac))
	}
	return addr
}
