#include <iostream>

#include "mac.hpp"
using namespace std;

int main(int argc, char const* argv[]) {
    MAC mac1 = MAC("11223344556677881122334455667788");
    MAC mac2 = MAC("11223344556677881122334455667788");
    string M = "Hello";
    cout << "HMAC: " << mac1.HMAC(M) << endl << (mac1.HMAC(M) == mac2.HMAC(M)) << endl;
    cout << "CBCMAC: " << mac1.CBCMAC(M) << (mac1.CBCMAC(M) == mac2.CBCMAC(M)) << endl;
    cout << "CMAC: " << mac1.CMAC(M) << (mac1.CMAC(M) == mac2.CMAC(M)) << endl;
    return 0;
}
