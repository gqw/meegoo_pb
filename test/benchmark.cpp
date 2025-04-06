#include <gtest/gtest.h>

#include "impl/util.h"
#include "test.pb.h"
#include "test_struct.h"

#include <chrono>

#include <meegoopb/pb.h>
#include <test_struct_refl.h>


constexpr uint64_t TEST_COUNT = 10'000'000;

TEST(benchmark, pb) {
    auto before = std::chrono::steady_clock::now();
    std::string buf;
    for (int i = 0; i < TEST_COUNT; ++i) {
        pb::Test test;
        test.set_i32(-100);
        test.set_i64(-100);
        test.set_u32(100);
        test.set_u64(100);
        test.set_si32(-100);
        test.set_si64(-100);
        test.set_fx32(100);
        test.set_fx64(100);
        test.set_sfx32(-100);
        test.set_sfx64(-100);
        test.set_bl(true);
        test.set_f32(100.0f);
        test.set_d64(100.0);
        test.set_str("100");
        test.set_bs({1, 2, 3});
        test.add_vec(3);
        test.add_vec(4);
        test.add_vec(5);
        test.mutable_mp()->insert({1, 2});
        test.mutable_mp()->insert({3, 4});
        test.mutable_test()->set_i32(-100);
        test.set_obj_str("100");
        pb::SubTest sub;
        sub.set_i32(-100);
        // test.mutable_gqw_any()->PackFrom(sub);
    
        buf.resize(test.ByteSizeLong());
        test.SerializeToArray(buf.data(), buf.size());
        test.ParseFromArray(buf.data(), buf.size());
    }
    auto after = std::chrono::steady_clock::now();
    std::cout << "pb cost: " << (after - before).count() << std::endl;

}


TEST(benchmark, pb2) {
    auto before = std::chrono::steady_clock::now();
    std::string buf;

    pb::Test test;
    test.set_i32(-100);
    test.set_i64(-100);
    test.set_u32(100);
    test.set_u64(100);
    test.set_si32(-100);
    test.set_si64(-100);
    test.set_fx32(100);
    test.set_fx64(100);
    test.set_sfx32(-100);
    test.set_sfx64(-100);
    test.set_bl(true);
    test.set_f32(100.0f);
    test.set_d64(100.0);
    test.set_str("100");
    test.set_bs({1, 2, 3});
    test.add_vec(3);
    test.add_vec(4);
    test.add_vec(5);
    test.mutable_mp()->insert({1, 2});
    test.mutable_mp()->insert({3, 4});
    test.mutable_test()->set_i32(-100);
    test.set_obj_str("100");
    pb::SubTest sub;
    sub.set_i32(-100);
    // test.mutable_gqw_any()->PackFrom(sub);
    
    for (int i = 0; i < TEST_COUNT; ++i) {
        pb::Test test2;
        buf.resize(test.ByteSizeLong());
        test.SerializeToArray(buf.data(), buf.size());
        test.ParseFromArray(buf.data(), buf.size());
    }
    auto after = std::chrono::steady_clock::now();
    std::cout << "pb cost: " << (after - before).count() << std::endl;

}

void check_eq(const TestAllData& l, const TestAllData& r) {
    ASSERT_EQ(l.i32, r.i32);
    ASSERT_EQ(l.i32, r.i32);                    
    ASSERT_EQ(l.i64, r.i64);                    
    ASSERT_EQ(l.u32, r.u32);                    
    ASSERT_EQ(l.u64, r.u64);                    
    ASSERT_EQ(l.si32, r.si32);                  
    ASSERT_EQ(l.si64, r.si64);                  
    ASSERT_EQ(l.fx32, r.fx32);                  
    ASSERT_EQ(l.fx64, r.fx64);                  
    ASSERT_EQ(l.sfx32, r.sfx32);                
    ASSERT_EQ(l.sfx64, r.sfx64);                
    ASSERT_EQ(l.bl, r.bl);                      
    ASSERT_EQ(l.f32, r.f32);                    
    ASSERT_EQ(l.d64, r.d64);                    
    ASSERT_EQ(l.str, r.str);                    
    ASSERT_EQ(l.bs, r.bs);                      
    ASSERT_EQ(l.vec, r.vec);                    
    ASSERT_EQ(l.mp, r.mp);                      
    ASSERT_EQ(l.substruct.i32, r.substruct.i32);        
    ASSERT_EQ(l.variant, r.variant);            
}

TEST(benchmark, meegoo) {
    auto before = std::chrono::steady_clock::now();
    std::string buf;

    TestAllData data, data2;
    data.i32 = -100;
    data.i64 = -100;
    data.u32 = 100;
    data.u64 = 100;
    data.si32 = -100;
    data.si64 = -100;
    data.fx32 = 100;
    data.fx64 = 100;
    data.sfx32 = -100;
    data.sfx64 = -100;
    data.bl = true;
    data.f32 = 100.1f;
    data.d64 = 100.1;
    data.str = "100";
    data.bs = {1, 2, 3};
    data.vec = {3, 4, 5};
    data.mp = {{1, 2}, {3, 4}};
    data.substruct = {-100};
    data.variant = std::string("100");
    data.i32_2 = 200;
    data.any = {{"type.googleapis.com/pb.SubTest", -100}};
    
    for (int i = 0; i < TEST_COUNT; ++i) {
        buf.resize(meegoo::pb::pb_size(data));
        meegoo::pb::to_pb(data, buf);
        meegoo::pb::from_pb(data2, buf);
    }
    auto after = std::chrono::steady_clock::now();
    std::cout << "cost: " << (after - before).count() << std::endl;

}