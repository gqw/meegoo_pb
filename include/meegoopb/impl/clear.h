#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <type_traits>

#include <meegoopb/define.h>
#include <meegoopb/impl/trait.h>
#include <meegoopb/impl/util.h>

namespace meegoo::pb {

template <typename ValueType>
inline static void clear_pb_field(ValueType &field_value) {
    using FieldType = std::remove_const_t<std::remove_reference_t<ValueType>>;
    if constexpr (std::is_arithmetic_v<FieldType>) {
        field_value = 0;
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sint32_t>
        || std::is_same_v<FieldType, meegoo::pb::sint64_t>
        || std::is_same_v<FieldType, meegoo::pb::fixed32_t>
        || std::is_same_v<FieldType, meegoo::pb::fixed64_t>
        || std::is_same_v<FieldType, meegoo::pb::sfixed32_t>
        || std::is_same_v<FieldType, meegoo::pb::sfixed64_t>) {
        field_value.val = 0;
    } else if constexpr (std::is_same_v<FieldType, std::string> ||
                         std::is_same_v<FieldType, meegoo::pb::bytes> ||
                         std::is_same_v<FieldType, std::vector<int8_t>> ||
                         std::is_same_v<FieldType, std::vector<uint8_t>>) {
        field_value.clear();
    } else if constexpr (meegoo::pb::is_sequence_container<FieldType>::value) {
        field_value.clear();
    } else if constexpr (meegoo::pb::is_associat_container<FieldType>::value) {
        field_value.clear();
    } else if constexpr (is_refl_struct_v<FieldType>) {
        
    } else if constexpr (is_variant_v<FieldType>) {
        
    } else if constexpr (is_array_v<FieldType>) {
        for (size_t i = 0; i < std::tuple_size_v<FieldType>; ++i) {
            field_value[i] = {};
        }
    } else {
        
    }
}


template <typename T>
inline static void clear_pb(T &t) {
    refl_visit_tp_members(t, [](T& t, auto &...args) {
        (clear_pb_field(t.*args.offset),...);
    });
}

} // namespace meegoo::pb
