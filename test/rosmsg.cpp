#include <gtest/gtest.h>

#include "impl/util.h"
#include "test.pb.h"
#include "test_struct.h"

#include <chrono>

#include <meegoopb/pb.h>
#include <test_struct_refl.h>


constexpr uint64_t TEST_COUNT = 10'000'000;


TEST(rosmsg, all) {
    std::string buf;

    RosMsgData data, data2;
    data.i8 = -100;
    data.i16 = -100;
    data.i32 = 100;
    data.i64 = 100;
    data.ui8 = -100;
    data.ui16 = -100;
    data.ui32 = 100;
    data.ui64 = 100;

    data.f32 = -100;
    data.d64 = -100;

    data.str = true;
    data.byte_arry = {1, 2, 3};
    data.byte_vec = {4, 5, 6};
    data.uint32_vec = {7, 8, 9};
    data.str_vec = {"10", "20", "30"};
    
    for (int i = 0; i < TEST_COUNT; ++i) {
        buf.resize(meegoo::pb::pb_size(data));
        meegoo::pb::to_pb(data, buf);
        meegoo::pb::from_pb(data2, buf);
    }
}