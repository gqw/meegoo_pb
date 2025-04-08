#pragma once

#include "define.h"
#include "impl/util.h"
#include <cstdint>
#include <iostream>
#include <meegoopb/impl/payload_stream.h>
#include <meegoopb/impl/trait.h>
#include <type_traits>
#include <utility>

namespace meegoo::pb {

template<typename T, typename TupleType, size_t ...I>
static bool find_and_decode(T& t, TupleType& tp, PayloadStream&  in,  
        uint32_t field_num, std::index_sequence<I...>) {
    bool found = false;
    (([&found, field_num](auto& t, auto& tp, PayloadStream&  in){
        auto& trait = std::get<I>(tp);
        if (trait.read_key() == field_num) {
            found = true;
            decode_pb_field_value(trait, t.*trait.offset, in);
        }
    })(t, tp, in), ...);

    if (found == false) {
        std::cout << "field not found, num: " << field_num << std::endl;
    }
    return found;
}

template <typename T>
inline static void decode_pb_field(T& t, PayloadStream &in) {
    constexpr auto tp_trait = refl_offset_to_tuple<T>();
    // constexpr auto trait_unmap = read_tuple_to_map(tp_trait);
    constexpr auto Size = std::tuple_size_v<decltype(tp_trait)>;

    while (!in.empty()) {
        uint32_t tag = 0;
        in.decode_varint(tag);
        uint32_t field_num = tag >> 3;
        find_and_decode(t, tp_trait, in, field_num, std::make_index_sequence<Size>());
       //  std::cout << " decode field_number: " << field_num << std::endl;
        // if (trait_unmap.find(field_num) == trait_unmap.end()) {
        //     std::cout << "field_num not found, " << field_num << std::endl;
        //     continue;
        // }
        // auto trait = trait_unmap.at(field_num);
        // std::visit([&t, &in, field_num](auto& trait){
        //     decode_pb_field_value(trait, t.*trait.offset, in);
        // }, trait);
    }
}

template <typename TraitType, typename FieldType>
inline static void decode_pb_field_value(TraitType& trait, FieldType& field_value,  PayloadStream &in) {

    if constexpr (std::is_same_v<FieldType, int32_t> 
        || std::is_same_v<FieldType, int8_t>
        || std::is_same_v<FieldType, int16_t>) {
        uint64_t val = 0;
        in.decode_varint(reinterpret_cast<uint64_t&>(val));
        field_value = val;
    } else if constexpr (std::is_same_v<FieldType, int64_t>) {
        in.decode_varint(reinterpret_cast<uint64_t&>(field_value));
    } else if constexpr (std::is_same_v<FieldType, uint32_t> 
        || std::is_same_v<FieldType, uint8_t>
        || std::is_same_v<FieldType, uint16_t>) {
        in.decode_varint(reinterpret_cast<uint32_t&>(field_value));
    } else if constexpr (std::is_same_v<FieldType, uint64_t>) {
        in.decode_varint(field_value);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sint32_t>) {
        in.decode_varint(reinterpret_cast<uint32_t&>(field_value.val));
        field_value = ZigZagDecode32(field_value);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sint64_t>) {
        in.decode_varint(reinterpret_cast<uint64_t&>(field_value.val));
        field_value = ZigZagDecode32(field_value);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::fixed32_t>) {
        in.decode_fixed32(reinterpret_cast<uint32_t&>(field_value.val));
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::fixed64_t>) {
        in.decode_fixed64(reinterpret_cast<uint64_t&>(field_value.val));
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sfixed32_t>) {
        in.decode_fixed32(reinterpret_cast<uint32_t&>(field_value.val));
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sfixed64_t>) {
        in.decode_fixed64(reinterpret_cast<uint64_t&>(field_value.val));
    } else if constexpr (std::is_same_v<FieldType, bool>) {
        field_value = (*in.read(1) != 0);
    } else if constexpr (std::is_same_v<FieldType, float>) {
        in.decode_float(field_value);
    } else if constexpr (std::is_same_v<FieldType, double>) {
        in.decode_double(field_value);
    } else if constexpr (std::is_same_v<FieldType, std::string>
        || std::is_same_v<FieldType, meegoo::pb::bytes>
        || std::is_same_v<FieldType, std::vector<int8_t>>
        || std::is_same_v<FieldType, std::vector<uint8_t>>
        || (meegoo::pb::is_array_v<FieldType> && sizeof(FieldType) == 1)) {
        uint64_t len = 0;
        in.decode_varint(len);
        uint8_t* buf = in.read(len);
        if constexpr (!meegoo::pb::is_array_v<FieldType>) {
            field_value.resize(len);
        }
        memcpy(field_value.data(), buf, len);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::bytes> 
        || (meegoo::pb::is_array_v<FieldType> && sizeof(FieldType) > 1)) {
        uint64_t len = 0;
        in.decode_varint(len);
        uint8_t* buf = in.read(len);
        if constexpr (!meegoo::pb::is_array_v<FieldType>) {
            field_value.resize(len);
        }
        memcpy(field_value.data(), buf, len);
    }else if constexpr (meegoo::pb::is_sequence_container<FieldType>::value) {
        field_value.clear();
        uint64_t len = 0;
        in.decode_varint(len);
        static thread_local PayloadStream sub_in;
        sub_in.reset(in.read(len), len);
        while (!sub_in.empty()) {
            typename FieldType::value_type value{};
            decode_pb_field_value(trait, value, sub_in);
            field_value.push_back(value);
        }
    } else if constexpr (meegoo::pb::is_associat_container<FieldType>::value) {
        uint64_t len = 0;
        in.decode_varint(len);
        typename FieldType::key_type key;
        typename FieldType::mapped_type value;
        uint64_t key_tag = 0;
        in.decode_varint(key_tag);
        decode_pb_field_value(trait, key, in);
        uint64_t value_tag = 0;
        in.decode_varint(value_tag);
        decode_pb_field_value(trait, value, in);
        field_value.insert({key, value});
    } else if constexpr (is_refl_struct_v<FieldType>) {
        uint64_t len = 0;
        in.decode_varint(len);
        static thread_local PayloadStream sub_in;
        sub_in.reset(in.read(len), len);
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