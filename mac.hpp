#pragma once

#include <bitset>
#include <vector>
#include <string>

class MAC {
private:
    // 密钥
    std::bitset<128> K;
    // 外部填充常数（取block长度为128）
    std::bitset<128> opad;
    // 内部填充常数（取block长度为128）
    std::bitset<128> ipad;
    void init(std::string hexkey);
public:
    std::bitset<256> HMAC(std::string M);
    std::bitset<128> CMAC(std::string M);
    std::bitset<128> CBCMAC(std::string M);
    MAC();
    MAC(std::string hexkey);
};