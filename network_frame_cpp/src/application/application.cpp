#include "application/application.h"

namespace application {

Data::Data(const std::string& payload) 
    : payload_(payload.begin(), payload.end()) {
}

Data::Data(const std::vector<uint8_t>& payload) 
    : payload_(payload) {
}

std::vector<uint8_t> Data::getPayload() const {
    return payload_;
}

std::string Data::getPayloadString() const {
    return std::string(payload_.begin(), payload_.end());
}

size_t Data::size() const {
    return payload_.size();
}

} // namespace application
