#include <meegoopb/pb.h>
#include "test_struct.h"
#include "test_struct_refl.h"
#include <iostream>
#include <bitset>
#include <variant>

#include <frozen/map.h>
#include <frozen/unordered_map.h>
#include <frozen/set.h>

using namespace meegoo::pb;

int main() {
    TestAllData data;
    //  1,i32: 00001,000 10011100 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111 00000001
    data.i32 = -100;
    //  2,i64: 00010,000 10011100 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111 00000001
    data.i64 = -100;
    //  3,u32: 00011,000 01100100
    data.u32 = 100;
    //  4,u64: 00100,000 01100100
    data.u64 = 100;
    //  5,si32: 00101,000 11000111 00000001
    data.si32 = -100;
    //  6,si64: 00110,000 11000111 00000001
    data.si64 = -100;
    //  7,fx32: 00111,101 10011100 11111111 11111111 11111111
    data.fx32 = 100;
    //  8,fx64: 01000,001 10011100 11111111 11111111 11111111 11111111 11111111 11111111 11111111
    data.fx64 = 100;
    // 9,sfx32: 01001,101 10011100 11111111 11111111 11111111
    data.sfx32 = -100;
    //10,sfx64: 01010,001 10011100 11111111 11111111 11111111 11111111 11111111 11111111 11111111
    data.sfx64 = -100;
    //   11,bl: 01011,000 00000001
    data.bl = true;
    //  12,f32: 01100,101 01100100 00000000 00000000 00000000
    data.f32 = 100.0f;
    //  13,d64: 01101,001 01100100 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    data.d64 = 100.0;
    //  14,str: 01110,010 00000011 00110001 00110000 00110000
    data.str = "100";
    //  15,bs:  01111,010 00000011 00000001 00000010 00000011
    data.bs = {1, 2, 3};
    //  16,vec: 10000,000 00000001 00000011 10000000 00000001 00000100 10000000 00000001 00000101
    data.vec = {3, 4, 5};
    //  17,mp:
    data.mp = {{1, 2}, {3, 4}};
    //  18,test:
    data.substruct = {100, 100, 10.0f};
    //  20,variant struct:
    data.variant = std::string("100");
    data.i32_2 = -200;
    // 21,any:
    data.any = {{"type.googleapis.com/pb.SubTest", 100}};

    std::string out;
    out.resize(pb_size(data));
    std::cout << "*** pb_size: " << out.size() << std::endl;
    auto ret = to_pb(data, out);
    std::cout << "size: " << out.size() << std::endl;
    for (size_t i = 0; i < out.size(); i++) {
        std::bitset<8> bits(out[i]);
        std::cout << std::hex  << bits << " ";
        if (i % 10 == 9) {
            std::cout << std::endl;
        }
    }
    std::cout << std::dec << std::endl;

    TestAllData data2;
    from_pb(data2, out);
    std::cout << "i32: " << data2.i32 << std::endl;
    std::cout << "si32: " << data2.si32 << std::endl;
    std::cout << "i32_2: " << data2.i32_2 << std::endl;
    return 0;
}