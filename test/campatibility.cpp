#include <gtest/gtest.h>

#include "impl/util.h"
#include "test.pb.h"
#include "test_struct.h"

#include <chrono>

#include <meegoopb/pb.h>
#include <test_struct_refl.h>


constexpr uint64_t TEST_COUNT = 100;

TEST(cts, all) {
    auto before = std::chrono::steady_clock::now();
    std::string buf;

    pb::Test pb, pb2;
    pb.set_i32(-100);
    pb.set_i64(-100);
    pb.set_u32(100);
    pb.set_u64(100);
    pb.set_si32(-100);
    pb.set_si64(-100);
    pb.set_fx32(100);
    pb.set_fx64(100);
    pb.set_sfx32(-100);
    pb.set_sfx64(-100);
    pb.set_bl(true);
    pb.set_f32(100.1f);
    pb.set_d64(100.1);
    pb.set_str("100");
    pb.set_bs({1, 2, 3});
    pb.add_vec(3);
    pb.add_vec(4);
    pb.add_vec(5);
    pb.mutable_mp()->insert({1, 2});
    pb.mutable_mp()->insert({3, 4});
    pb.mutable_test()->set_i32(-100);
    pb.set_obj_str("100");
    pb::SubTest sub;
    sub.set_i32(-100);
    // pb.mutable_gqw_any()->PackFrom(sub);    

    for (int i = 0; i < TEST_COUNT; ++i) {
        buf.resize(pb.ByteSizeLong());
        pb.SerializeToArray(buf.data(), buf.size());
        // meegoo::pb::DebugPrint(buf);
        TestAllData meegoo;
        meegoo::pb::from_pb(meegoo, buf);

        ASSERT_EQ(pb.i32(), meegoo.i32);
        ASSERT_EQ(pb.i64(), meegoo.i64);
        ASSERT_EQ(pb.u32(), meegoo.u32);
        ASSERT_EQ(pb.u64(), meegoo.u64);
        ASSERT_EQ(pb.si32(), meegoo.si32);
        ASSERT_EQ(pb.si64(), meegoo.si64);
        ASSERT_EQ(pb.fx32(), meegoo.fx32);
        ASSERT_EQ(pb.fx64(), meegoo.fx64);
        ASSERT_EQ(pb.sfx32(), meegoo.sfx32);
        ASSERT_EQ(pb.sfx64(), meegoo.sfx64);
        ASSERT_EQ(pb.bl(), meegoo.bl);
        ASSERT_EQ(pb.f32(), meegoo.f32);
        ASSERT_EQ(pb.d64(), meegoo.d64);
        ASSERT_EQ(pb.str(), meegoo.str);
        ASSERT_EQ(pb.bs().size(), meegoo.bs.size());
        ASSERT_EQ(pb.vec().size(), meegoo.vec.size());
        ASSERT_EQ(pb.mp().size(), meegoo.mp.size());
        ASSERT_EQ(pb.mp().at(1), meegoo.mp.at(1));
        ASSERT_EQ(pb.obj_str(), std::get<std::string>(meegoo.variant));
        ASSERT_EQ(std::get<std::string>(meegoo.variant), "100");

        buf.resize(meegoo::pb::pb_size(meegoo));
        meegoo::pb::to_pb(meegoo, buf);

        pb2.ParseFromArray(buf.data(), buf.size());


        // pb compare with pb2
        ASSERT_EQ(pb.i32(), pb2.i32());
        ASSERT_EQ(pb.i64(), pb2.i64());
        ASSERT_EQ(pb.u32(), pb2.u32());
        ASSERT_EQ(pb.u64(), pb2.u64());
        ASSERT_EQ(pb.si32(), pb2.si32());
        ASSERT_EQ(pb.si64(), pb2.si64());
        ASSERT_EQ(pb.fx32(), pb2.fx32());
        ASSERT_EQ(pb.fx64(), pb2.fx64());
        ASSERT_EQ(pb.sfx32(), pb2.sfx32());
        ASSERT_EQ(pb.sfx64(), pb2.sfx64());
        ASSERT_EQ(pb.bl(), pb2.bl());
        ASSERT_EQ(pb.f32(), pb2.f32());
        ASSERT_EQ(pb.d64(), pb2.d64());
        ASSERT_EQ(pb.str(), pb2.str());
        ASSERT_EQ(pb.bs(), pb2.bs());
        ASSERT_EQ(pb.vec().size(), pb2.vec().size());
        ASSERT_EQ(pb.mp().size(), pb2.mp().size());
        ASSERT_EQ(pb.mp().at(1), pb2.mp().at(1));
        ASSERT_EQ(pb.mp().at(3), pb2.mp().at(3));
        ASSERT_EQ(pb.obj_str(), pb2.obj_str());
        ASSERT_EQ(pb2.obj_str(), "100");



    }
    auto after = std::chrono::steady_clock::now();
    std::cout << "pb cost: " << (after - before).count() << std::endl;
}

