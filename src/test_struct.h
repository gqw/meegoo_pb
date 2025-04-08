#pragma once

#include <cstdint>
#include <meegoopb/define.h>

#include <string>
#include <map>
#include <vector>
#include <variant>


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


struct RosMsgData {
    int8_t              i8       =   0;
    int16_t             i16      =   0;
    int32_t             i32      =   0;
    int64_t             i64      =   0;

    uint8_t              ui8       =   0;
    uint16_t             ui16      =   0;
    uint32_t             ui32      =   0;
    uint64_t             ui64      =   0;

    float                f32      =   0.0f;
    double               d64      =   0.0;

    std::string          str      =   "";
    std::array<uint8_t, 10>     byte_arry = {0};
    std::array<int8_t, 10>      ubyte_arry = {0};
    std::vector<uint8_t>        byte_vec = {0};
    std::vector<int8_t>         ubyte_vec = {0};

    std::vector<uint32_t>       uint32_vec = {0};
    std::vector<std::string>    str_vec = {0};
};