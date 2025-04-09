#pragma once


#include <iostream>
#include <meegoopb/define.h>
#include <meegoopb/impl/util.h>
#include <meegoopb/impl/calc_size.h>
#include <meegoopb/impl/payload_stream.h>
#include <meegoopb/impl/trait.h>
#include <tuple>
#include <type_traits>

namespace meegoo::pb {

template <typename T, typename WriteFunc>
inline static void encode_pb_number_field(WireType type, const T& value,
            uint32_t field_number, WriteFunc write_value_func, PayloadStream &out) {
    uint8_t* target = out.cur();
    target = WriteTagToArray(field_number, type, target);
    target = (*write_value_func)(value, target);
    out.set_cur(target);
}

template <typename TraitType, typename ValueType, bool check_def = true>
inline static void encode_pb_field(TraitType& trait, size_t field_number, size_t tag_len,
        const ValueType& field_value, PayloadStream &out) {
    using FieldType = std::remove_const_t<std::remove_reference_t<ValueType>>;            
    if constexpr (std::is_same_v<FieldType, int32_t> 
        || std::is_same_v<FieldType, int8_t>
        || std::is_same_v<FieldType, int16_t>) {
        if constexpr (check_def) {
            if (field_value == 0) [[unlikely]]
                return;
        }
        // int32_t 比较特殊，需要转换成 int64_t 处理
        encode_pb_number_field(WireType::WIRETYPE_VARINT, field_value,
                field_number, WriteVarint64ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, int64_t>) {
        if constexpr (check_def) {
            if (field_value == 0) [[unlikely]]
                return;
        }
        encode_pb_number_field(WireType::WIRETYPE_VARINT, field_value,
            field_number, WriteVarint64ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, uint32_t> 
        || std::is_same_v<FieldType, uint8_t>
        || std::is_same_v<FieldType, uint16_t>) {
        if constexpr (check_def) {
            if (field_value == 0) [[unlikely]]
                return;
        }
        encode_pb_number_field(WireType::WIRETYPE_VARINT, field_value,
            field_number, WriteVarint32ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, uint64_t>) {
        if constexpr (check_def) {
            if (field_value == 0) [[unlikely]]
                return;
        }
        encode_pb_number_field(WireType::WIRETYPE_VARINT, field_value,
            field_number, WriteVarint64ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sint32_t>) {
        if constexpr (check_def) {
            if (field_value.val == 0) [[unlikely]]
                return;
        }
        encode_pb_number_field(WireType::WIRETYPE_VARINT, ZigZagEncode32(field_value),
            field_number, WriteVarint32ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sint64_t>) {
        if constexpr (check_def) {
            if (field_value.val == 0) [[unlikely]]
                return;
        }
        encode_pb_number_field(WireType::WIRETYPE_VARINT, ZigZagEncode64(field_value),
            field_number, WriteVarint64ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::fixed32_t>) {
        if constexpr (check_def) {
            if (field_value.val == 0) [[unlikely]]
                return;
        }
        encode_pb_number_field(WireType::WIRETYPE_FIXED32, field_value,
            field_number, WriteLittleEndian32ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::fixed64_t>) {
        if constexpr (check_def) {
            if (field_value.val == 0) [[unlikely]]
                return;
        }
        encode_pb_number_field(WireType::WIRETYPE_FIXED64, field_value,
            field_number, WriteLittleEndian64ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sfixed32_t>) {
        if constexpr (check_def) {
            if (field_value.val == 0) [[unlikely]]
                return;
        }
        encode_pb_number_field(WireType::WIRETYPE_FIXED32, static_cast<uint32_t>(field_value),
            field_number, WriteLittleEndian32ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sfixed64_t>) {
        if constexpr (check_def) {
            if (field_value.val == 0) [[unlikely]]
                return;
        }
        encode_pb_number_field(WireType::WIRETYPE_FIXED64, static_cast<uint64_t>(field_value),
            field_number, WriteLittleEndian64ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, bool>) {
        if constexpr (check_def) {
            if (field_value == 0) [[unlikely]]
                return;
        }
        encode_pb_number_field(WireType::WIRETYPE_VARINT, field_value,
            field_number, WriteVarint32ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, float>) {
        if constexpr (check_def) {
            if (field_value == 0.0f) [[unlikely]]
                return;
        }
        encode_pb_number_field(WireType::WIRETYPE_FIXED32, field_value,
            field_number, WriteLittleEndianFloatToArray, out);
    } else if constexpr (std::is_same_v<FieldType, double>) {
        if constexpr (check_def) {
            if (field_value == 0.0) [[unlikely]]
                return;
        }
        encode_pb_number_field(WireType::WIRETYPE_FIXED64, field_value,
            field_number, WriteLittleEndianDoubleToArray, out);
    } else if constexpr (std::is_same_v<FieldType, std::string>
            || std::is_same_v<FieldType, meegoo::pb::bytes>
            || std::is_same_v<FieldType, std::vector<int8_t>>
            || std::is_same_v<FieldType, std::vector<uint8_t>>
            || (meegoo::pb::is_byte_array_v<FieldType>)) {
        if constexpr (check_def) {
            if (field_value.empty()) [[unlikely]]
                return;
        }                
        uint8_t* target = out.cur();
        target = WriteTagToArray(field_number, WireType::WIRETYPE_LENGTH_DELIMITED, target);
        const FieldType& value = field_value;
        // if constexpr (meegoo::pb::is_array_v<FieldType>) {
        //     constexpr size_t value_size = std::tuple_size_v<FieldType>;
        //     target = WriteVarint32ToArray(value_size, target);
        //     std::memcpy(target, value.data(), value_size);
        //     out.set_cur(target + value_size);
        // } else 
        {
            target = WriteVarint32ToArray(value.size(), target);
            std::memcpy(target, value.data(), value.size());
            out.set_cur(target + value.size());
        }
    } else if constexpr (meegoo::pb::is_sequence_container<FieldType>::value 
        || ((meegoo::pb::is_array_v<FieldType> 
            && !meegoo::pb::is_byte_array_v<FieldType>))) {
        if constexpr (check_def) {
            if (field_value.empty()) [[unlikely]]
                return;
        } 

        using ContainerValueType = typename FieldType::value_type;
        constexpr bool is_tlv = !std::is_arithmetic_v<ContainerValueType>;
        size_t count = field_value.size();
        if (is_tlv) { // 是否是 t-l-v 类型的
            for (size_t i = 0; i < count; ++i) {
                // size_t value_len = 0;
                // calc_pb_size(trait, 0, 0, field_value.at(i), value_len, false);
                uint8_t* target = out.cur();
                target = WriteTagToArray(field_number, WireType::WIRETYPE_LENGTH_DELIMITED, target);
                const FieldType& value = field_value;
                // target = WriteVarint32ToArray(value_len, target);
                out.set_cur(target);
                encode_pb_field(trait, 0, tag_len, field_value.at(i), out);                     
            }
        } else {
            size_t value_len = 0;
            for (size_t i = 0; i < field_value.size(); ++i) {
                calc_pb_size(trait, 0, 0, field_value[i], value_len);
            }

            uint8_t* target = out.cur();
            target = WriteTagToArray(field_number, WireType::WIRETYPE_LENGTH_DELIMITED, target);
            const FieldType& value = field_value;
            target = WriteVarint32ToArray(value_len, target);
            out.set_cur(target);
            for (size_t i = 0; i < value.size(); ++i) {
                encode_pb_field(trait, 0, tag_len, value[i], out);
            }
        }
    } else if constexpr (meegoo::pb::is_associat_container<FieldType>::value) {
        if constexpr (check_def) {
            if (field_value.empty()) [[unlikely]]
                return;
        } 
        for (const auto& [key, value] : field_value) {
            size_t key_len = 0;
            meegoo::pb::calc_pb_size(trait, 1, 1, key, key_len);

            size_t value_len = 0;
            meegoo::pb::calc_pb_size(trait, 2, 1, value, value_len);

            uint8_t* target = out.cur();
            target = WriteTagToArray(field_number, WireType::WIRETYPE_LENGTH_DELIMITED, target);
            target = WriteVarint32ToArray(key_len + value_len, target);
            out.set_cur(target);
            encode_pb_field(trait, 1, 1, key, out);
            encode_pb_field(trait, 2, 1, value, out);
        }
    } else if constexpr (is_refl_struct_v<FieldType>) {
        size_t value_len = [](auto& t){
            size_t len = 0;
            refl_visit_tp_members(t, [&len](const auto& t, const auto &...args) {
                (calc_pb_size(args, args.field_number, args.tag_len, t.*args.offset, len),...);
            });
            return len;
        }(field_value);
        if (value_len == 0)
            return;

        uint8_t* target = out.cur();
        target = WriteTagToArray(field_number, WireType::WIRETYPE_LENGTH_DELIMITED, target);
        target = WriteVarint32ToArray(value_len, target);
        out.set_cur(target);
        {
            refl_visit_tp_members(field_value, [&out](const auto& t, const auto &...args) {
                (encode_pb_field(args, args.field_number, args.tag_len, t.*args.offset, out), ...);
            });
        }
    } else if constexpr (is_variant_v<FieldType>) {
        if (field_value.index() == trait.variant_index) {
            auto variant_number = field_number + field_value.index();
            auto tag_len = calc_tag_size_constexpr(variant_number  << 3);

            std::visit([variant_number, tag_len, &out, &trait](auto&& v){
                encode_pb_field(trait, variant_number, tag_len, v, out);
            }, field_value);
        }
    }
    else {
        std::cerr << "unsupported field type" << std::endl;
    }
}



} // namespace meegoo::pb