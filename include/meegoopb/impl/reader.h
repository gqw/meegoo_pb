#pragma once

#include "define.h"
#include "impl/util.h"
#include <cstdint>
#include <iostream>
#include <meegoopb/impl/payload_stream.h>
#include <meegoopb/impl/trait.h>
#include <type_traits>

namespace meegoo::pb {

template <typename T>
inline static void decode_pb_field(T& t, PayloadStream &in) {
    constexpr auto tp_trait = refl_offset_to_tuple<T>();
    constexpr auto trait_unmap = read_tuple_to_map(tp_trait);
    constexpr auto Size = std::tuple_size_v<decltype(tp_trait)>;

    while (!in.empty()) {
        uint64_t tag = in.decode_varint();
        uint32_t field_num = tag >> 3;
        std::cout << " decode field_number: " << field_num << std::endl;
        auto trait = trait_unmap.at(field_num);
        std::visit([&t, &in, field_num](auto& trait){
            decode_pb_field_value(trait, t.*trait.offset, in);
        }, trait);
    }
}

template <typename TraitType, typename FieldType>
inline static void decode_pb_field_value(TraitType& trait, FieldType& field_value,  PayloadStream &in) {

    if constexpr (std::is_same_v<FieldType, int32_t>) {
        field_value = in.decode_varint();
    } else if constexpr (std::is_same_v<FieldType, int64_t>) {
        field_value = in.decode_varint();
    } else if constexpr (std::is_same_v<FieldType, uint32_t>) {
        field_value = in.decode_varint();
    } else if constexpr (std::is_same_v<FieldType, uint64_t>) {
        field_value = in.decode_varint();
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sint32_t>) {
        field_value = ZigZagDecode32(in.decode_varint());
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sint64_t>) {
        field_value = ZigZagDecode32(in.decode_varint());
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::fixed32_t>) {
        field_value = in.decode_fixed32();
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::fixed64_t>) {
        field_value = in.decode_fixed64();
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sfixed32_t>) {
        field_value = in.decode_fixed32();
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sfixed64_t>) {
        field_value = in.decode_fixed64();
    } else if constexpr (std::is_same_v<FieldType, bool>) {
        field_value = (*in.read(1) != 0);
    } else if constexpr (std::is_same_v<FieldType, float>) {
        uint32_t value = in.decode_fixed32();
        memcpy(&field_value, &value, 4);
    } else if constexpr (std::is_same_v<FieldType, double>) {
        uint64_t value = in.decode_fixed64();
        memcpy(&field_value, &value, 8);
    } else if constexpr (std::is_same_v<FieldType, std::string>) {
        uint32_t len = in.decode_varint();
        uint8_t* buf = in.read(len);
        field_value.assign(reinterpret_cast<char*>(buf), reinterpret_cast<char*>(buf + len));
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::bytes>) {
        uint32_t len = in.decode_varint();
        uint8_t* buf = in.read(len);
        field_value.assign(buf, buf + len);
    }else if constexpr (meegoo::pb::is_sequence_container<FieldType>::value) {
        uint32_t len = in.decode_varint();
        PayloadStream sub_in(in.read(len), len);
        while (!sub_in.empty()) {
            typename FieldType::value_type value;
            decode_pb_field_value(trait, value, sub_in);
            field_value.push_back(value);
        }
    } else if constexpr (meegoo::pb::is_associat_container<FieldType>::value) {
        uint32_t len = in.decode_varint();
        typename FieldType::key_type key;
        typename FieldType::mapped_type value;
        uint64_t key_tag = in.decode_varint();
        decode_pb_field_value(trait, key, in);
        uint64_t value_tag = in.decode_varint();
        decode_pb_field_value(trait, key, in);
        field_value.insert({key, value});
    } else if constexpr (is_refl_struct_v<FieldType>) {
        uint32_t len = in.decode_varint();
        PayloadStream sub_in(in.read(len), len);
        decode_pb_field(field_value, sub_in);
    } else if constexpr (is_variant_v<FieldType>) {
        typename TraitType::value_type value;
        decode_pb_field_value(trait, value, in);
        field_value = std::move(value);
    }
    else {
        std::cerr << "unsupported field type" << std::endl;
    }
    // std::cout << std::dec << "decode_pb_field_value: " << field_value << std::endl;
}



} // namespace meegoo::pb