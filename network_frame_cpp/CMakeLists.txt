cmake_minimum_required(VERSION 3.10)
project(network_frame VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/application/application.cpp
    src/transport/udp.cpp
    src/network/ipv4.cpp
    src/datalink/ethernet.cpp
    src/sender/sender.cpp
    src/receiver/receiver.cpp
)

# Create executable
add_executable(network_frame ${SOURCES})

# Installation
install(TARGETS network_frame RUNTIME DESTINATION bin)
