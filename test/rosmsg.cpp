#include <array>
#include <gtest/gtest.h>

#include "impl/util.h"
#include "test.pb.h"
#include "test_struct.h"

#include <chrono>

#include <meegoopb/pb.h>
#include <test_struct_refl.h>


constexpr uint64_t TEST_COUNT = 10;


TEST(rosmsg, all) {
    std::string buf;
    buf.clear();

    ::pb::RosMsg pb, pb2;
    RosMsgData data, data2;
    // 00001,000 10011100 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111 00000001
    //    |        |
    //    |        -------------- (value: -100)
    //    ----------------------- (field number: 1, wire type: 0, varint变成类型)    
    data.i8 = -100;
    // 00010,000 10011100 11111111 11111111 11111111 11111111 11111111 11111111 11111111 11111111 00000001
    //    |        |
    //    |        -------------- (value: -100)
    //    ----------------------- (field number: 2, wire type: 0, varint变成类型)    
    data.i16 = -100;
    // 00011,000 01100100
    //    |        |
    //    |        -------------- (value: 100)
    //    ----------------------- (field number: 3, wire type: 0, varint变成类型)
    data.i32 = 100;
    // 00100,000 01100100
    //    |        |
    //    |        -------------- (value: 100)
    //    ----------------------- (field number: 4, wire type: 0, varint变成类型)    
    data.i64 = 100;
    // 00101,000 01100100
    //    |        |
    //    |        -------------- (value: 100)
    //    ----------------------- (field number: 5, wire type: 0, varint变成类型)     
    data.ui8 = 100;
    // 00110,000 01100100
    //    |        |
    //    |        -------------- (value: 100)
    //    ----------------------- (field number: 6, wire type: 0, varint变成类型)     
    data.ui16 = 100;
    // 00111,000 01100100
    //    |        |
    //    |        -------------- (value: 100)
    //    ----------------------- (field number: 7, wire type: 0, varint变成类型)     
    data.ui32 = 100;
    // 01000,000 01100100
    //    |        |
    //    |        -------------- (value: 100)
    //    ----------------------- (field number: 8, wire type: 0, varint变成类型)     
    data.ui64 = 100;
    // 01001,101 00000000 00000000 11001000 11000010
    //    |        |         |          |        |
    //    |        ---------(value: -100.0) ------
    //    ----------------------- (field number: 9, type: 5, 固定4字节)
    data.f32 = -100;
    // 01010,001 00000000 00000000 00000000 00000000 00000000 00000000 01011001 11000000
    //    |        |         |          |        |
    //    |        ---------(value: -100.0) ------
    //    ----------------------- (field number: 10, type: 1, 固定8字节)
    data.d64 = -100;
    // 01011,010 00000011 00110001 00110000 00110000
    //    |        |         |          |        |
    //    |        |         ---- (value: "100") -
    //    |        -------------- (len: 3)
    //    ----------------------- (field number: 11, type: 2, Length-delimited)
    data.str = "100";
    // 01100,010 00001010 00000001 00000010 00000011 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    //    |        |         |          |        |
    //    |        |         ---- (value: 1, 2, 3) -
    //    |        -------------- (len: 10)
    //    ----------------------- (field number: 12, type: 2, Length-delimited)
    data.byte_arry = {1, 2, 3};
    // 01101,010 00001010 00000001 00000010 00000011 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    //    |        |         |          |        |
    //    |        |         ---- (value: 1, 2, 3) -
    //    |        -------------- (len: 10)
    //    ----------------------- (field number: 13, type: 2, Length-delimited) 
    data.ubyte_arry = {1, 2, 3};
    // 01110,010 00001010 00000100 00000101 00000110
    //    |        |         |          |        |
    //    |        |         ---- (value: 4, 5, 6) -
    //    |        -------------- (len: 3)
    //    ----------------------- (field number: 14, type: 2, Length-delimited)          
    data.byte_vec = {4, 5, 6};
    // 01111,010 00000011 00000100 00000101 00000110  
    //    |        |         |          |        |
    //    |        |         ---- (value: 7, 8, 9) -
    //    |        -------------- (len: 3)
    //    ----------------------- (field number: 15, type: 2, Length-delimited)  
    data.ubyte_vec = {4, 5, 6};
    // 10000,010 00000001 00000011 00000111 00001000 00001001
    //    |       |         |         |          |        |
    //    |       |         |         ---- (value: 7, 8, 9) -
    //    |       |         -------------- (len: 3)
    //    ----------------------- (field number: 16, type: 2, Length-delimited)         
    data.uint32_vec = {7, 8, 9};
    // 10001,010 00000001 00001001 00000010 00110001 00110000 00000010 00110010 00110000 00000010 00110011 00110000
    //    |         |        |         |          |        |       |          |        |     |          |        |
    //    |         |        |     （len: 2）     '1'      '0' （len: 2）     '2'      '0' （len: 2）   '3'      '0'
    //    |         |        -------------- (len: 9)
    //    ----------------------- (field number: 17, type: 2, Length-delimited)     
    data.str_vec = {"10", "20", "30"};
    // 10010,010 00000001 00000101 00000001 00000010 00000011 00000000 00000000
    //    |         |        |         |        |        |       |          |    
    //    |         |        |         1       2        3        0          0
    //    |         |        -------------- (len: 5)
    //    ----------------------- (field number: 18, type: 2, Length-delimited)     
    data.uint32_array = {1,2,3};

    pb::RosTest2 test2;
    test2.add_str_vec("10");
    test2.add_str_vec("20");
    test2.add_str_vec("30");
    std::string str = test2.SerializeAsString();
    meegoo::pb::DebugPrint(str);
    pb::RosTest2 test2_1;
    test2_1.ParseFromString(str);

    pb::RosTest3 test3;
    test3.add_uint32_vec(7);
    test3.add_uint32_vec(8);
    test3.add_uint32_vec(9);
    std::string str2 = test3.SerializeAsString();
    meegoo::pb::DebugPrint(str2);

    for (int i = 0; i < 1; ++i) {
        buf.resize(meegoo::pb::pb_size(data));
        meegoo::pb::to_pb(data, buf);
        meegoo::pb::DebugPrint(buf, "meegoo to pb");
        pb.ParseFromArray(buf.data(), buf.size());
        meegoo::pb::DebugPrint(buf, "pb from meegoo");

        ASSERT_EQ(pb.i8(), data.i8);
        ASSERT_EQ(pb.i16(), data.i16);
        ASSERT_EQ(pb.i32(), data.i32);
        ASSERT_EQ(pb.i64(), data.i64);
        ASSERT_EQ(pb.ui8(), data.ui8);
        ASSERT_EQ(pb.ui16(), data.ui16);
        ASSERT_EQ(pb.ui32(), data.ui32);
        ASSERT_EQ(pb.ui64(), data.ui64);
        ASSERT_EQ(pb.f32(), data.f32);
        ASSERT_EQ(pb.d64(), data.d64);
        ASSERT_EQ(pb.str(), data.str);
        // ASSERT_EQ(pb.byte_arry().size(), data.byte_arry.size());
        ASSERT_EQ(pb.byte_vec().size(), data.byte_vec.size());
        ASSERT_EQ(pb.uint32_vec().size(), data.uint32_vec.size());
        ASSERT_EQ(pb.str_vec().size(), data.str_vec.size());

        pb.SerializeToArray(buf.data(), buf.size());
        meegoo::pb::DebugPrint(buf, "pb to buf");
        meegoo::pb::from_pb(data2, buf);
        meegoo::pb::to_pb(data2, buf);
        meegoo::pb::DebugPrint(buf, "meegoo to buf2");
        pb2.ParseFromArray(buf.data(), buf.size());
        ASSERT_EQ(pb2.i8(), data2.i8);
        ASSERT_EQ(pb2.i16(), data2.i16);
        ASSERT_EQ(pb2.i32(), data2.i32);
        ASSERT_EQ(pb2.i64(), data2.i64);
        ASSERT_EQ(pb2.ui8(), data2.ui8);
        ASSERT_EQ(pb2.ui16(), data2.ui16);
        ASSERT_EQ(pb2.ui32(), data2.ui32);
        ASSERT_EQ(pb2.ui64(), data2.ui64);
        ASSERT_EQ(pb2.f32(), data2.f32);
        ASSERT_EQ(pb2.d64(), data2.d64);
        ASSERT_EQ(pb2.str(), data2.str);
        // ASSERT_EQ(pb2.byte_arry().size(), data2.byte_arry.size());
        ASSERT_EQ(pb2.byte_vec().size(), data2.byte_vec.size());
        ASSERT_EQ(pb2.uint32_vec().size(), data2.uint32_vec.size());
        ASSERT_EQ(pb2.str_vec().size(), data2.str_vec.size());
    }
}

TEST(rosmsg, uint32_vec) {
    std::string buf;
    pb::RosTest3 test3;
    test3.add_uint32_vec(7);
    test3.add_uint32_vec(8);
    test3.add_uint32_vec(9);
    std::string str2 = test3.SerializeAsString();
    meegoo::pb::DebugPrint(str2);
}