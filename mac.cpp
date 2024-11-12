#include <map>

#include "lib/sm3.hpp"
#include "lib/padding.hpp"
#include "lib/operation.hpp"
#include "mac.hpp"

std::bitset<256> MAC::HMAC(std::string M) {
    SM3 sm3 = SM3();
    // HMAC(m, k) = H ((k ⊕ opad) | H((k ⊕ ipad) | m))
    return sm3.hash((this->K ^ this->opad).to_string() + sm3.hash((this->K ^ this->ipad).to_string()).to_string() + M);
}

std::bitset<128> MAC::CBCMAC(std::string M) {
    // 返回CBC模式加密时，IV为0的最后一个分组
    Padding pd = Padding(PaddingMode::PKCS7_Padding);
    Operation op = Operation(this->K, OperationMode::CBC);
    vector<bitset<128>> res = op.enc(pd.generateBlock(M), 0);
    return res[res.size() - 1];
}

std::bitset<128> MAC::CMAC(std::string M) {
    // SM4算法下的R常量，AES-128的R常量为0x87
    bitset<128> R = bitset<128>(0xc3);
    bitset<128> L = SM4(this->K).enc(0);
    bitset<128> K1, K2;
    vector<bitset<128>> res;
    // GF域乘法，可以查看AES-encryption
    if (L.test(127)) {
        K1 = (L << 1) ^ R;
    } else {
        K1 = L << 1;
    }
    if (K1.test(127)) {
        K2 = (K1 << 1) ^ R;
    } else {
        K2 = (K1 << 1);
    }
    if (M.size() % 16 == 0) {
        // 正好可以完成分组时不填充，使用K1
        Padding pd = Padding(PaddingMode::NoPadding);
        Operation op = Operation(this->K, OperationMode::CBC);
        res = op.enc(pd.generateBlock(M), 0);
        res[res.size() - 1] ^= K1;
    } else {
        // 不能完成分组时先添加比特1，再用比特0补齐，使用K2
        Padding pd = Padding(PaddingMode::ISO__7816_4__Padding);
        Operation op = Operation(this->K, OperationMode::CBC);
        res = op.enc(pd.generateBlock(M), 0);
        res[res.size() - 1] ^= K2;
    }
    // MSB函数用于从加密结果最后一个密文分组中提取指定位数，一般从高位提取，这里当就提取全部位了
    return res[res.size() - 1];
}

MAC::MAC() {
    this->init("11223344556677881122334455667788");
}

MAC::MAC(std::string hexkey) {
    this->init(hexkey);
}

void MAC::init(std::string hexkey) {
    std::map<char, std::string> hex2Bin = {
        {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"}, {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"},
        {'8', "1000"}, {'9', "1001"}, {'A', "1010"}, {'B', "1011"}, {'C', "1100"}, {'D', "1101"}, {'E', "1110"}, {'F', "1111"}
    };
    // 固定的
    string hexipad = "36", hexopad = "5C";
    for (int i = 0; i < 16; i++) {  // 每次填充一个字节
        std::string bin = "";
        bin += hex2Bin[hexkey[2 * i]];
        bin += hex2Bin[hexkey[2 * i + 1]];
        this->K <<= 8;
        this->K |= bitset<128>(bin);
        bin = hex2Bin[hexipad[0]];
        bin += hex2Bin[hexipad[1]];
        this->ipad <<= 8;
        this->ipad |= bitset<128>(bin);
        bin = hex2Bin[hexopad[0]];
        bin += hex2Bin[hexopad[1]];
        this->opad <<= 8;
        this->opad |= bitset<128>(bin);
    }
}
