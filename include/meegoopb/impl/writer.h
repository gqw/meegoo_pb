#pragma once


#include <iostream>
#include <meegoopb/define.h>
#include <meegoopb/impl/util.h>
#include <meegoopb/impl/calc_size.h>
#include <meegoopb/impl/payload_stream.h>
#include <meegoopb/impl/trait.h>
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

template <typename TraitType, typename FieldType, typename FieldNumberType, typename TagType>
inline static void encode_pb_field(TraitType& trait, FieldNumberType field_number, TagType tag_len,
        const FieldType& field_value, PayloadStream &out) {
    if constexpr (std::is_same_v<FieldType, int32_t>) {
        if (field_value == 0) [[unlikely]]
            return;
        // int32_t 比较特殊，需要转换成 int64_t 处理
        encode_pb_number_field(WireType::WIRETYPE_VARINT, field_value,
                field_number, WriteVarint64ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, int64_t>) {
        if (field_value == 0) [[unlikely]]
            return;
        encode_pb_number_field(WireType::WIRETYPE_VARINT, field_value,
            field_number, WriteVarint64ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, uint32_t>) {
        if (field_value == 0) [[unlikely]]
            return;
        encode_pb_number_field(WireType::WIRETYPE_VARINT, field_value,
            field_number, WriteVarint32ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, uint64_t>) {
        if (field_value == 0) [[unlikely]]
            return;
        encode_pb_number_field(WireType::WIRETYPE_VARINT, field_value,
            field_number, WriteVarint64ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sint32_t>) {
        if (field_value.val == 0) [[unlikely]]
            return;
        encode_pb_number_field(WireType::WIRETYPE_VARINT, ZigZagEncode32(field_value),
            field_number, WriteVarint32ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sint64_t>) {
        if (field_value.val == 0)  [[unlikely]]
            return;
        encode_pb_number_field(WireType::WIRETYPE_VARINT, ZigZagEncode64(field_value),
            field_number, WriteVarint64ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::fixed32_t>) {
        if (field_value.val == 0) [[unlikely]]
            return;
        encode_pb_number_field(WireType::WIRETYPE_FIXED32, field_value,
            field_number, WriteLittleEndian32ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::fixed64_t>) {
        if (field_value.val == 0) [[unlikely]]
            return;
        encode_pb_number_field(WireType::WIRETYPE_FIXED64, field_value,
            field_number, WriteLittleEndian64ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sfixed32_t>) {
        if (field_value.val == 0) [[unlikely]]
            return;
        encode_pb_number_field(WireType::WIRETYPE_FIXED32, static_cast<uint32_t>(field_value),
            field_number, WriteLittleEndian32ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sfixed64_t>) {
        if (field_value.val == 0) [[unlikely]]
            return;
        encode_pb_number_field(WireType::WIRETYPE_FIXED64, static_cast<uint64_t>(field_value),
            field_number, WriteLittleEndian64ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, bool>) {
        if (field_value == false)
            return;
        encode_pb_number_field(WireType::WIRETYPE_VARINT, field_value,
            field_number, WriteVarint32ToArray, out);
    } else if constexpr (std::is_same_v<FieldType, float>) {
        if (field_value == 0.0f) [[unlikely]]
            return;
        encode_pb_number_field(WireType::WIRETYPE_FIXED32, field_value,
            field_number, WriteLittleEndianFloatToArray, out);
    } else if constexpr (std::is_same_v<FieldType, double>) {
        if (field_value == 0.0) [[unlikely]]
            return;
        encode_pb_number_field(WireType::WIRETYPE_FIXED64, field_value,
            field_number, WriteLittleEndianDoubleToArray, out);
    } else if constexpr (std::is_same_v<FieldType, std::string>
            || std::is_same_v<FieldType, meegoo::pb::bytes>) {
        if (field_value.empty()) [[unlikely]]
            return;
        uint8_t* target = out.cur();
        target = WriteTagToArray(field_number, WireType::WIRETYPE_LENGTH_DELIMITED, target);
        const FieldType& value = field_value;
        target = WriteVarint32ToArray(value.size(), target);
        std::memcpy(target, value.data(), value.size());
        out.set_cur(target + value.size());
    } else if constexpr (meegoo::pb::is_sequence_container<FieldType>::value) {
        if (field_value.empty()) [[unlikely]]
            return;
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
    } else if constexpr (meegoo::pb::is_associat_container<FieldType>::value) {
        if (field_value.empty()) [[unlikely]]
            return;
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