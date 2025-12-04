#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include <vector>
#include <cstdint>

namespace application {

// 应用层数据类
class Data {
public:
    Data() = default;
    explicit Data(const std::string& payload);
    explicit Data(const std::vector<uint8_t>& payload);

    // 获取原始数据
    std::vector<uint8_t> getPayload() const;
    
    // 获取原始数据字符串形式
    std::string getPayloadString() const;
    
    // 获取数据大小
    size_t size() const;

private:
    std::vector<uint8_t> payload_;
};

} // namespace application

#endif // APPLICATION_H
