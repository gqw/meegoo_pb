#include "impl/util.h"
#include "test.pb.h"
#include <gtest/gtest.h>

#include <bitset>


TEST(Protobuf, TestAllTypes) {
    pb::Test test;
    // 00001,000 10011100 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111 00000001
    //    |        |
    //    |        -------------- (value: -100)
    //    ----------------------- (field number: 1, wire type: 0, varint变成类型)
    test.set_i32(-100);

    // 00010,000 10011100 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111 00000001
    //    |        |
    //    |        -------------- (value: -100)
    //    ----------------------- (field number: 2, wire type: 0, varint变成类型)
    test.set_i64(-100);

    // 00011,000 01100100
    //    |        |
    //    |        -------------- (value: 100)
    //    ----------------------- (field number: 3, wire type: 0, varint变成类型)
    test.set_u32(100);

    // 00100,000 01100100
    //    |        |
    //    |        -------------- (value: 100)
    //    ----------------------- (field number: 4, wire type: 0, varint变成类型)
    test.set_u64(100);

    // 00101,000 11000111 00000001
    //    |        |         |
    //    |        -------------- (value: 100)
    //    ----------------------- (field number: 5, wire type: 0, varint变成类型)
    test.set_si32(-100);

    // 00110,000 11000111 00000001
    //    |        |         |
    //    |        -------------- (value: 100)
    //    ----------------------- (field number: 6, wire type: 0, varint变成类型)
    test.set_si64(-100);

    // 00111,101 01100100 00000000 00000000 00000000
    //    |        |         |          |        |
    //    |        -------------- (value: 100) ---
    //    ----------------------- (field number: 7, type: 5, 固定4字节)
    test.set_fx32(100);

    // 01000,001 01100100 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    //    |        |         |          |        |      |        |         |          |
    //    |        -------------- (value: 100) ----------------------------------------
    //    ----------------------- (field number: 8, type: 1, 固定8字节)
    test.set_fx64(100);

    // 01001,101 01100100 00000000 00000000 00000000
    //    |        |         |          |        |
    //    |        -------------- (value: 100) ---
    //    ----------------------- (field number: 9, type: 5, 固定4字节)
    test.set_sfx32(100);

    // 01010,001 01100100 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    //    |        |         |          |        |      |        |         |          |
    //    |        -------------- (value: 100) ----------------------------------------
    //    ----------------------- (field number: 10, type: 1, 固定8字节)
    test.set_sfx64(100);

    // 01011,000 00000001
    //    |        |
    //    |        -------------- (value: 100)
    //    ----------------------- (field number: 11)
    test.set_bl(true);

    // 01100,101 00000000 00000000 11001000 01000010
    //    |        |         |          |        |
    //    |        -------------- (value: 100.0) -
    //    ----------------------- (field number: 12, type: 5, 固定4字节)
    test.set_f32(100.0);

    // 01101,001 00000000 00000000 00000000 00000000 00000000 00000000 01011001 01000000
    //    |        |         |          |        |      |        |         |          |
    //    |        -------------- (value: 100.0) --------------------------------------
    //    ----------------------- (field number: 13, type: 5, 固定4字节)
    test.set_d64(100.0);

    // 01110,010 00000011 00110001 00110000 00110000
    //    |        |         |          |        |
    //    |        |         ---- (value: "100") -
    //    |        -------------- (len: 3)
    //    ----------------------- (field number: 14, type: 2, Length-delimited)
    test.set_str("100");

    // 01111,010 00000101 00000001 00000010 00000011 00000100 00000101
    //    |        |         |          |        |       |        |
    //    |        |         ---- (value: 1,2,3,4,5) --------------
    //    |        -------------- (len: 5)
    //    ----------------------- (field number: 15, type: 2, Length-delimited)
    test.set_bs({1, 2, 3, 4, 5});

    // 10000010 00000001 00000011 00000011 00000100 00000101
    //    |        |         |          |        |       |
    //    |        |         |          ---(value: 3,4,5) --
    //    |        |         ---- (len: 3)
    //    ----------------------- (field number: 16, type: 2, Length-delimited)
    //
    // note： field number: 16, 根据 varint 编码规则, 需要用2个字节表示
    test.add_vec(3);
    test.add_vec(4);
    test.add_vec(5);

    // 10001,010 00000001 00000100 00001,000 00000011 00010,000 00000100
    // 10001,010 00000001 00000100 00001,000 00000001 00010,000 00000010
    //    |        |         |          |        |       |        |
    //    |        |         |          |        |       |        -- (real value: 2 & 4)
    //    |        |         |          |        |       ---- (value field number: 2, type: 2)
    //    |        |         |          |        ---- (key value: 3 & 1)
    //    |        |         |          ---- (key field number: 1, type: 2)
    //    |        |         ---- (len: 4)
    //    ----------------------- (field number: 17, type: 2, Length-delimited)
    test.mutable_mp()->insert({1, 2});
    test.mutable_mp()->insert({3, 4});

    // 10010,010 00000001 00000010 00001,000 01100100
    //    |        |         |          |        |
    //    |        |         |          |        ---- (value: 100)
    //    |        |         |          ---- (field number: 1, type: 0)
    //    |        |         ---- (len: 2)
    //    ----------------------- (field number: 18, type: 2, Length-delimited)
    test.mutable_test()->set_i32(100);

    // 10100,010 00000001 00000011 00110001 00110000 00110000
    //    |        |         |          |        |       |
    //    |        |         |          ---- (value: "100")
    //    |        |         ---- (len: 3)
    //    ----------------------- (field number: 20, type: 2, Length-delimited)
    test.set_obj_str("100");

    // 10101,010 00000001 00100100 00001,010 00011110 01110100 01111001 01110000 01100101
    //    |        |         |          |        |       |        |        |        |
    //    |        |         |          |        |       -- (value: "type.googleapis.com/pb.SubTest")
    //    |        |         |          |        ---- (value len: 30)
    //    |        |         |          ---- (field number: 1, type: 2, Length-delimited)
    //    |        |         ---- (len: 36)
    //    ----------------------- (field number: 21, type: 2, Length-delimited)
    // 00101110 01100111 01101111 01101111 01100111 01101100 01100101 01100001 01110000 01101001
    //    |        |         |          |        |       |        |        |        |      |
    //    ---------------- (value: "type..googleapis.com/pb.SubTest") ----------------------
    // 01110011 00101110 01100011 01101111 01101101 00101111 01110000 01100010 00101110 01010011
    //    |        |         |          |        |       |        |        |        |      |
    //    ---------------- (value: "type..googleapis.com/pb.SubTest") ----------------------
    // 01110101 01100010 01010100 01100101 01110011 01110100 00010010 00000010 00001000 01100100
    //    |        |         |          |        |       |       |       |        |        |
    //    ----- (value: ".googleapis.com/pb.SubTest") ---        |       |        |        - (value: 100)
    //                                                           |       |        - (field number: 2, type: 0)
    //                                                           |       ----- (value len: 2)
    //                                                            ---- (field number: 2, type: 2, Length-delimited)
    pb::SubTest sub;
    sub.set_i32(100);
    // default type_url: type.googleapis.com/pb.SubTest
    // test.mutable_gqw_any()->set_type_url("url");
    // test.mutable_gqw_any()->PackFrom(sub);

    auto size = test.ByteSizeLong();
    auto bytes = test.SerializeAsString();

    ASSERT_EQ(size, bytes.size());
    meegoo::pb::DebugPrint(bytes);

    test.ParseFromString(bytes);
    // size: 109
    // 00001000 01100100 00010000 01100100 00011000 01100100 00100000 01100100 00101000 11001000
    // 00000001 00110000 11001000 00000001 00111101 01100100 00000000 00000000 00000000 01000001
    // 01100100 00000000 00000000 00000000 00000000 00000000 00000000 00000000 01001101 01100100
    // 00000000 00000000 00000000 01010001 01100100 00000000 00000000 00000000 00000000 00000000
    // 00000000 00000000 01011000 00000001 01100101 00000000 00000000 11001000 01000010 01101001
    // 00000000 00000000 00000000 00000000 00000000 00000000 01011001 01000000 01110010 00000011
    // 00110001 00110000 00110000 01111010 00000101 00000001 00000010 00000011 00000100 00000101
    // 10000010 00000001 00000011 00000011 00000100 00000101 10001010 00000001 00000100 00001000
    // 00000011 00010000 00000100 10001010 00000001 00000100 00001000 00000001 00010000 00000010
    // 10010010 00000001 00000010 00001000 01100100 10100010 00000001 00000011 00110001 00110000
    // 00110000 10101010 00000001 00000101 00001010 00000011 00110001 00110000 00110000
}


TEST(Protobuf, TestNagative) {
    pb::Test2 test;


    // 00011,000 01100100
    //    |        |
    //    |        -------------- (value: 100)
    //    ----------------------- (field number: 3)
    test.set_u32(100);

    // 00110,000 11000111 00000001
    //    |        |         |
    //    |        -------------- (value: -100)
    //    ----------------------- (field number: 6)
    test.set_si64(-100);

    // 00000011 10110101 11000,000
    // 11000,000 10110101 00000011 10011100 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111 00000001
    //    |         |       |          |       |         |       |         |         |        |       |         |       |
    //    |         |       |          --------------------------- (value: -100) ----------------------------------------
    //    ----------------------- (field number: 7000 = binary 0000011 0110101 1000)
    //
    // note： field number: 7000, 根据 varint 编码规则, 需要用3个字节表示
    test.set_i32(-100);


    auto size = test.ByteSizeLong();
    auto bytes = test.SerializeAsString();
    test.ParseFromString(bytes);

    ASSERT_EQ(size, bytes.size());
    std::cout << "size: " << size << std::endl;
    for (size_t i = 0; i < size; i++) {
        std::bitset<8> bits(bytes[i]);
        std::cout << std::hex  << bits << " ";
        if (i % 10 == 9) {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;

    // size: 12
    // 00011000 01100100 00110000 11000111 00000001 11000000 10110101 00000011 10011100 11111111
    // 11111111 11111111 11111111 11111111 11111111 11111111 11111111 00000001
}


TEST(Protobuf, TestRepeated) {
    pb::Test3 test;


    // 00001,010 00001100 00000011 11111100 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111 00000001 00000101
    //    |        |         |          |        |       |      |        |         |          |        |       |         |          |
    //    |        |         -----------------------(value: 3,-4,5) ------------------------------------------------------------------
    //    |        ---- (len: 12)
    //    --------------(field number: 1, type: 2, Length-delimited)
    test.add_vec(3);
    test.add_vec(-4);
    test.add_vec(5);


    auto size = test.ByteSizeLong();
    auto bytes = test.SerializeAsString();

    ASSERT_EQ(size, bytes.size());
    std::cout << "size: " << std::dec << size << std::endl;
    for (size_t i = 0; i < size; i++) {
        std::bitset<8> bits(bytes[i]);
        std::cout << std::hex  << bits << " ";
        if (i % 10 == 9) {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;

    pb::Test3 test2;
    test2.ParseFromString(bytes);

    auto size2 = test2.ByteSizeLong();
    auto bytes2 = test2.SerializeAsString();
    ASSERT_EQ(size, size2);
    ASSERT_EQ(bytes, bytes2);
    std::cout << test2.vec_size() << std::endl;

    // size: 14
    // 00001010 00001100 00000011 11111100 11111111 11111111 11111111 11111111 11111111 11111111
    // 11111111 11111111 00000001 00000101
}