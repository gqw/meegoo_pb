#include <tuple>
#include <utility>

#include "test_struct.h"
#include <meegoopb/impl/trait.h>
#include <meegoopb/frozen/algorithm.h>

namespace meegoo::pb {

/////////////////////////////// SubTest2 ///////////////////////////////
template <>
[[maybe_unused]] inline constexpr decltype(auto)
refl_offset_to_tuple<SubTest2>() {
    constexpr auto tp = std::make_tuple(
        field_trait_info{&SubTest2::i32, "i32", 1}
        // field_trait_info{&SubTest2::i64, "i64", 2},
        // variant_trait_info(&SubTest2::variant, "variant", 3, 0),
        // variant_trait_info(&SubTest2::variant, "variant", 3, 1)
    );
    return tp;
}

/////////////////////////////// TestAllData ///////////////////////////////
template <>
[[maybe_unused]] inline constexpr decltype(auto)
refl_offset_to_tuple<TestAllData>() {
    constexpr auto tp = std::make_tuple(
        field_trait_info{&TestAllData::i32, "i32", 1},
        field_trait_info{&TestAllData::i64, "i64", 2},
        field_trait_info{&TestAllData::u32, "u32", 3},
        field_trait_info{&TestAllData::u64, "u64", 4},
        field_trait_info{&TestAllData::si32, "si32", 5},
        field_trait_info{&TestAllData::si64, "si64", 6},
        field_trait_info{&TestAllData::fx32, "fx32", 7},
        field_trait_info{&TestAllData::fx64, "fx64", 8},
        field_trait_info{&TestAllData::sfx32, "sfx32", 9},
        field_trait_info{&TestAllData::sfx64, "sfx64", 10},
        field_trait_info{&TestAllData::bl, "bl", 11},
        field_trait_info{&TestAllData::f32, "f32", 12},
        field_trait_info{&TestAllData::d64, "d64", 13},
        field_trait_info{&TestAllData::str, "str", 14},
        field_trait_info{&TestAllData::bs, "bs", 15},
        field_trait_info{&TestAllData::vec, "vec", 16},
        field_trait_info{&TestAllData::mp, "mp", 17},
        field_trait_info{&TestAllData::substruct, "test", 18},
        variant_trait_info(&TestAllData::variant, "variant", 19, 0),
        variant_trait_info(&TestAllData::variant, "variant", 19, 1)
        // ,
        // field_trait_info{&TestAllData::any, "any", 21}
    );
    return tp;
}

/////////////////////////////// TestAllData ///////////////////////////////
template <>
[[maybe_unused]] inline constexpr decltype(auto)
refl_offset_to_tuple<RosMsgData>() {
    constexpr auto tp = std::make_tuple(
        field_trait_info{&RosMsgData::i8, "i8", 1},
        field_trait_info{&RosMsgData::i16, "i16", 2},
        field_trait_info{&RosMsgData::i32, "i32", 3},
        field_trait_info{&RosMsgData::i64, "i64", 4},
        field_trait_info{&RosMsgData::ui8, "ui8", 5},
        field_trait_info{&RosMsgData::ui16, "ui16", 6},
        field_trait_info{&RosMsgData::ui32, "ui32", 7},
        field_trait_info{&RosMsgData::ui64, "ui64", 8},
        field_trait_info{&RosMsgData::f32, "f32", 9},
        field_trait_info{&RosMsgData::d64, "d64", 10},
        field_trait_info{&RosMsgData::str, "str", 11},
        field_trait_info{&RosMsgData::byte_arry, "byte_arry", 12},
        field_trait_info{&RosMsgData::ubyte_arry, "ubyte_arry", 13},
        field_trait_info{&RosMsgData::byte_vec, "byte_vec", 14},
        field_trait_info{&RosMsgData::ubyte_vec, "ubyte_vec", 15},
        field_trait_info{&RosMsgData::uint32_vec, "uint32_vec", 16},
        field_trait_info{&RosMsgData::str_vec, "str_vec", 17},
        field_trait_info{&RosMsgData::uint32_array, "uint32_array", 18}
    );
    return tp;
}

} // namespace meegoo::pb
