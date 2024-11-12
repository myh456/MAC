// 和SM3-hash一样的
#pragma once

#include <vector>
#include <bitset>
#include <string>
using namespace std;

class SM3 {
private:
    bitset<256> IV;
    vector<bitset<32>> T;
    vector<bitset<32>> W = vector<bitset<32>>(68, 0x00);
    vector<bitset<32>> Wp = vector<bitset<32>>(64, 0x00);
    vector<bitset<8>> padding(string M);
    bitset<256> compression(vector<bitset<8>> m);
    bitset<256> CF(bitset<256> vi, bitset<512> bi);
    void extension(bitset<512> bi);
public:
    SM3();
    bitset<256> hash(string str);
};