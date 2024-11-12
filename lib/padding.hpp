// 和blockCipher_padding_operation一样的
#pragma once

#include <vector>
#include <bitset>
#include <string>

enum PaddingMode {
    NoPadding,
    ZeroPadding,
    RandomPadding,
    ANSI__X_923__Padding,
    ISO__7816_4__Padding,
    PKCS5_Padding,
    PKCS7_Padding
};

class Padding {
private:
    int paddingMode = -1;
public:
    std::vector<std::bitset<128>> generateBlock(std::string plain);
    std::string restoreBlock(std::vector<std::bitset<128>> block);
    void setPaddingMode(int mode);
    Padding();
    Padding(int mode);
};
