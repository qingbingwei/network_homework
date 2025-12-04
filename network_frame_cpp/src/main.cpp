#include <iostream>
#include <string>
#include <cstring>
#include "application/application.h"
#include "sender/sender.h"
#include "receiver/receiver.h"

const std::string DEFAULT_FILENAME = "packet.bin";
const std::string DEFAULT_MESSAGE = "Hello Teacher";

void printUsage() {
    std::cout << "Network Protocol Simulation Program (C++)" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  ./network_frame send    - Encapsulate data and save to packet.bin" << std::endl;
    std::cout << "  ./network_frame receive - Read packet.bin and decapsulate" << std::endl;
    std::cout << "  ./network_frame demo    - Full demo (encapsulate + decapsulate)" << std::endl;
}

void runSender(const std::string& message) {
    std::cout << "========================================" << std::endl;
    std::cout << "       Encapsulation Module (Sender)" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    application::Data appData(message);
    
    auto config = sender::Sender::defaultConfig();
    sender::Sender s(config);
    
    s.encapsulateAndSave(appData, DEFAULT_FILENAME);
    
    std::cout << std::endl << "Encapsulation complete!" << std::endl;
}

void runReceiver(const std::string& filename) {
    std::cout << "========================================" << std::endl;
    std::cout << "       Decapsulation Module (Receiver)" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    receiver::Receiver r;
    auto parsed = r.decapsulateFromFile(filename);
    
    std::cout << std::endl << "========================================" << std::endl;
    std::cout << "       Decapsulation Result Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Original message: " << parsed.applicationData->getPayloadString() << std::endl;
}

void runDemo(const std::string& message) {
    std::cout << "========================================" << std::endl;
    std::cout << "  Network Protocol Simulation - Demo" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    // Encapsulation
    std::cout << "========================================" << std::endl;
    std::cout << "         Part 1: Encapsulation" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    application::Data appData(message);
    
    auto config = sender::Sender::defaultConfig();
    sender::Sender s(config);
    
    s.encapsulateAndSave(appData, DEFAULT_FILENAME);
    
    // Decapsulation
    std::cout << std::endl << std::endl << "========================================" << std::endl;
    std::cout << "         Part 2: Decapsulation" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;
    
    receiver::Receiver r;
    auto parsed = r.decapsulateFromFile(DEFAULT_FILENAME);
    
    // Verification
    std::cout << std::endl << "========================================" << std::endl;
    std::cout << "         Verification Result" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Original sent message: " << message << std::endl;
    std::cout << "Decapsulated message:  " << parsed.applicationData->getPayloadString() << std::endl;
    
    if (message == parsed.applicationData->getPayloadString()) {
        std::cout << "Verification SUCCESS: Messages match!" << std::endl;
    } else {
        std::cout << "Verification FAILED: Messages don't match!" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 0;
    }
    
    std::string command = argv[1];
    std::string message = (argc > 2) ? argv[2] : DEFAULT_MESSAGE;
    
    try {
        if (command == "send") {
            runSender(message);
        } else if (command == "receive") {
            std::string filename = (argc > 2) ? argv[2] : DEFAULT_FILENAME;
            runReceiver(filename);
        } else if (command == "demo") {
            runDemo(message);
        } else {
            printUsage();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
