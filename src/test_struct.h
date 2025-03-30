#pragma once

#include <meegoopb/define.h>

#include <string>
#include <map>
#include <vector>
#include <variant>

struct pair_t {
  pair_t() = default;
  pair_t(int a, int b) : x(a), y(b) {}
  int x;
  int y;
};

struct SubTest2 {
    int32_t              i32      =   0;
    int64_t              i64      =   0;
    std::variant<float, std::string> variant;
};

struct TestAllData {
    int32_t              i32      =   0;
    int64_t              i64      =   0;
    uint32_t             u32      =   0;
    uint64_t             u64      =   0;
    meegoo::pb::sint32_t         si32     =   0;
    meegoo::pb::sint64_t         si64     =   0;
    meegoo::pb::fixed32_t        fx32     =   0;
    meegoo::pb::fixed64_t        fx64     =   0;
    meegoo::pb::sfixed32_t       sfx32    =   0;
    meegoo::pb::sfixed64_t       sfx64    =   0;
    bool                 bl       =   true;
    float                f32      =   0.0f;
    double               d64      =   0.0;
    std::string          str      =   "";
    meegoo::pb::bytes            bs       =   {};
    std::vector<int32_t> vec      =   {};
    std::map<int32_t, int32_t>  mp =  {};
    SubTest2             substruct =  {};
    std::variant<float, std::string> variant;
    meegoo::pb::google_any<int32_t>             any;
    int32_t              i32_2      =   0;
};