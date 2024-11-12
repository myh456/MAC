// 和blockCipher_padding_operation一样的
#include "padding.hpp"
#include <iostream>

std::bitset<128> strToBitset(std::string str) {
    std::bitset<128> bs;
    for (int index = 0; index < 16; index++) {
        bs <<= 8;
        bs |= str[index];
    }
    return bs;
}

std::vector<std::bitset<128>> Padding::generateBlock(std::string plain) {
    std::vector<std::bitset<128>> res;
    while (true) {
        if (plain.size() > 16) {
            res.push_back(strToBitset(plain.substr(0, 16)));
            plain.erase(0, 16);
        } else {
            switch (this->paddingMode) {
            case PaddingMode::NoPadding:
                if (plain.size() != 16) {
                    std::cerr << "乖，别闹" << std::endl;
                    exit(0);
                }
                res.push_back(strToBitset(plain));
                break;

            case PaddingMode::ZeroPadding:
                res.push_back(strToBitset(plain + std::string(16 - plain.size(), 0x00)));
                break;

            case PaddingMode::RandomPadding: {
                    std::string randStr = "";
                    while (randStr.size() + plain.size() != 16) {
                        srand(time(NULL));
                        randStr += rand() % 0xff;
                    }
                    res.push_back(strToBitset(plain + randStr));
                } break;

            case PaddingMode::ANSI__X_923__Padding:
                if (plain.size() == 16) {
                    res.push_back(strToBitset(plain));
                    res.push_back(strToBitset(std::string(15, 0x00) + (char)0x0f));
                } else {
                    res.push_back(strToBitset(plain + std::string(15 - plain.size(), 0x00) + (char)(15 - plain.size())));
                }
                break;

            case PaddingMode::ISO__7816_4__Padding:
                plain += 0x80;
                if (plain.size() > 16) {
                    res.push_back(strToBitset(plain.substr(0, 16)));
                    plain.erase(0, 16);
                }
                res.push_back(strToBitset(plain + std::string(16 - plain.size(), 0x00)));
                break;
            case PaddingMode::PKCS5_Padding:
                std::cout << "SM4算法明文分组需要16字节, 帮你选PKCS7 Padding了";

            case PaddingMode::PKCS7_Padding:
                if (plain.size() == 16) {
                    res.push_back(strToBitset(plain));
                    res.push_back(strToBitset(std::string(16, 0x10)));
                } else {
                    res.push_back(strToBitset(plain + std::string(16 - plain.size(), 16 - plain.size())));
                }
                break;

            default:
                std::cerr << "先选个模式再用吧" << std::endl;
                exit(0);
            }
            break;
        }
    }
    return res;
}

std::string Padding::restoreBlock(std::vector<std::bitset<128>> block) {
    std::string str = "";
    for (int i = 0; i < block.size() - 1; i++) {
        for (int j = 0; j < 16; j++) {
            str += (char)(block[i] >> (120 - j * 8) & std::bitset<128>(0xff)).to_ulong();
        }
    }
    switch (this->paddingMode) {
    case PaddingMode::NoPadding:

    case PaddingMode::ZeroPadding:

    case PaddingMode::RandomPadding:
        for (int j = 0; j < 16; j++) {
            str += (char)(*(block.end() - 1) >> (120 - j * 8) & std::bitset<128>(0xff)).to_ulong();
        }
        break;

    case PaddingMode::ANSI__X_923__Padding: {
            int len = 15 - (*(block.end() - 1) & std::bitset<128>(0xff)).to_ulong();
            for (int j = 0; j < len; j++) {
                str += (char)(*(block.end() - 1) >> (120 - j * 8) & std::bitset<128>(0xff)).to_ulong();
            }
        } break;

    case PaddingMode::ISO__7816_4__Padding: {
            int len = 0;
            for (int j = 0; j < 16; j++) {
                if (!(*(block.end() - 1) >> (j * 8) & std::bitset<128>(0xff)).to_ulong()) {
                    len = 15 - j;
                }
            }
            for (int j = 0; j < len; j++) {
                str += (char)(*(block.end() - 1) >> (120 - j * 8) & std::bitset<128>(0xff)).to_ulong();
            }
        } break;

    case PaddingMode::PKCS5_Padding:

    case PaddingMode::PKCS7_Padding: {
            int len = 16 - (*(block.end() - 1) & std::bitset<128>(0xff)).to_ulong();
            for (int j = 0; j < len; j++) {
                str += (char)(*(block.end() - 1) >> (120 - j * 8) & std::bitset<128>(0xff)).to_ulong();
            }
        } break;

    default:
        std::cerr << "???";
        exit(0);
    }
    return str;
}

void Padding::setPaddingMode(int mode) {
    this->paddingMode = mode;
}

Padding::Padding() { }
Padding::Padding(int mode) : paddingMode(mode) { }