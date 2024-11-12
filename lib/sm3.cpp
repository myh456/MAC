// 和SM3-hash一样的，把utility.cpp的代码整进来了
#include "sm3.hpp"

#define LLL(bs, pos, n) bitset<n>((bs) << (pos % n) | (bs) >> (n - (pos % n)))
#define FF0(X, Y, Z) ((X) ^ (Y) ^ (Z))
#define FF1(X, Y, Z) ((((X) & (Y)) | ((X) & (Z)) | ((Y) & (Z))))
#define GG0(X, Y, Z) ((X) ^ (Y) ^ (Z))
#define GG1(X, Y, Z) (((X) & (Y)) | (~(X) & (Z)))
#define P0(X) (X) ^ LLL(X, 9, 32) ^ LLL(X, 17, 32)
#define P1(X) (X) ^ LLL(X, 15, 32) ^ LLL(X, 23, 32)

vector<bitset<8>> SM3::padding(string M) {
    vector<bitset<8>> m = vector<bitset<8>>(M.begin(), M.end());
    long lenM = m.size();
    m.push_back(0x80);
    while (m.size() % 64 != 56) {
        m.push_back(bitset<8>(0x00));
    }
    for (int i = 0; i < 8; i++) {
        m.push_back(lenM >> (56 - i * 8) & 0xff);
    }
    return m;
}

bitset<256> SM3::compression(vector<bitset<8>> m) {
    vector<bitset<512>> b;
    vector<bitset<256>> v;
    for (int i = 0; i < m.size() / 64; i++) {
        b.push_back(bitset<512>(0));
        for (int j = 0; j < 64; j++) {
            b[i] <<= 8;
            b[i] |= m[m.size() - 1 - 64 * i - j].to_ulong();
        }
    }
    v.push_back(this->IV);
    for (int i = 0; i < b.size(); i++) {
        v.push_back(this->CF(v[i], b[i]));
    }
    return v[v.size() - 1];
}

bitset<256> SM3::CF(bitset<256> vi, bitset<512> bi) {
    bitset<256> abcdefg, temp = vi;
    bitset<32> a, b, c, d, e, f, g, h;
    bitset<32> ss1, ss2, tt1, tt2;
    this->extension(bi);
    h = (temp & bitset<256>(0xffffffff)).to_ullong(); temp >>= 32;
    g = (temp & bitset<256>(0xffffffff)).to_ullong(); temp >>= 32;
    f = (temp & bitset<256>(0xffffffff)).to_ullong(); temp >>= 32;
    e = (temp & bitset<256>(0xffffffff)).to_ullong(); temp >>= 32;
    d = (temp & bitset<256>(0xffffffff)).to_ullong(); temp >>= 32;
    c = (temp & bitset<256>(0xffffffff)).to_ullong(); temp >>= 32;
    b = (temp & bitset<256>(0xffffffff)).to_ullong(); temp >>= 32;
    a = (temp & bitset<256>(0xffffffff)).to_ullong();
    for (int i = 0; i < 64; i++) {
        ss1 = LLL(LLL(a, 12, 32) + e + LLL(this->T[i], i, 32), 7, 32);
        ss2 = ss1 + LLL(a, 12, 32);
        if (i < 16) {
            tt1 = FF0(a, b, c) + d + ss2 + this->Wp[i];
            tt2 = GG0(a, b, c) + h + ss1 + this->W[i];
        } else {
            tt1 = FF1(a, b, c) + d + ss2 + this->Wp[i];
            tt2 = GG1(a, b, c) + h + ss1 + this->W[i];
        }
        d = c;
        c = LLL(b, 9, 32);
        b = a;
        a = tt1;
        h = g;
        g = LLL(f, 19, 32);
        f = e;
        e = P0(tt2);
    }
    abcdefg |= a.to_ulong(); abcdefg <<= 32;
    abcdefg |= b.to_ulong(); abcdefg <<= 32;
    abcdefg |= c.to_ulong(); abcdefg <<= 32;
    abcdefg |= d.to_ulong(); abcdefg <<= 32;
    abcdefg |= e.to_ulong(); abcdefg <<= 32;
    abcdefg |= f.to_ulong(); abcdefg <<= 32;
    abcdefg |= g.to_ulong(); abcdefg <<= 32;
    abcdefg |= h.to_ulong();
    return abcdefg ^ vi;
}

void SM3::extension(bitset<512> bi) {
    for (int i = 0; i < 16; i++) {
        this->W[i] = (bi & bitset<512>(0xffffffff)).to_ullong();
        bi >>= 32;
    }
    for (int i = 16; i < 68; i++) {
        this->W[i] = P1(this->W[i - 16] ^ this->W[i - 9] ^ LLL(this->W[i - 3], 15, 32)) ^ LLL(this->W[i - 13], 7, 32) ^ this->W[i - 6];
    }
    for (int i = 0; i < 64; i++) {
        this->Wp[i] = this->W[i] ^ this->W[i + 4];
    }
}

SM3::SM3() {
    int i;
    string binary;
    string hex = "7380166F4914B2B9172442D7DA8A0600A96F30BC163138AAE38DEE4DB0FB0E4E";

    for (char hexChar : hex) {
        switch (hexChar) {
        case '0': binary += "0000"; break;
        case '1': binary += "0001"; break;
        case '2': binary += "0010"; break;
        case '3': binary += "0011"; break;
        case '4': binary += "0100"; break;
        case '5': binary += "0101"; break;
        case '6': binary += "0110"; break;
        case '7': binary += "0111"; break;
        case '8': binary += "1000"; break;
        case '9': binary += "1001"; break;
        case 'A': binary += "1010"; break;
        case 'B': binary += "1011"; break;
        case 'C': binary += "1100"; break;
        case 'D': binary += "1101"; break;
        case 'E': binary += "1110"; break;
        case 'F': binary += "1111"; break;
        }
    }
    this->IV = bitset<256>(binary);
    for (i = 0; i < 16; i++) {
        this->T.push_back(bitset<32>(0x79cc4519));
    }
    for (i = 16; i < 64; i++) {
        this->T.push_back(bitset<32>(0x7a879d8a));
    }
}

bitset<256> SM3::hash(string str) {
    return this->compression(this->padding(str));
}

template <size_t N>
std::bitset<N> operator+(const std::bitset<N>& lhs, const std::bitset<N>& rhs) {
    std::bitset<N> result;
    bool carry = false;

    for (size_t i = 0; i < N; ++i) {
        bool bit1 = lhs[i];
        bool bit2 = rhs[i];

        bool sum = bit1 ^ bit2 ^ carry;
        carry = (bit1 && bit2) || (carry && (bit1 ^ bit2));

        result[i] = sum;
    }

    return result;
}