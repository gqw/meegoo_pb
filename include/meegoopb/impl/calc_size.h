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

template <typename T> size_t calc_pb_varint_size(T field_value) { return 0; }

template <typename TraitType, typename FieldType, typename FieldNumberType, typename TagType>
inline static void calc_pb_size( TraitType trait, FieldNumberType field_number, TagType tag_len,
                                const FieldType &field_value, size_t &len) {
    size_t old_len = len;
    if constexpr (std::is_same_v<FieldType, int32_t>) {
        if (field_value == 0)
            return;
        len += tag_len;
        len += variant_uint64_size(field_value);
    } else if constexpr (std::is_same_v<FieldType, int64_t>) {
        if (field_value == 0)
            return;
        len += tag_len;
        len += variant_uint64_size(field_value);
    } else if constexpr (std::is_same_v<FieldType, uint32_t>) {
        if (field_value == 0)
            return;
        len += tag_len;
        len += variant_uint32_size(field_value);
    } else if constexpr (std::is_same_v<FieldType, uint64_t>) {
        if (field_value == 0)
            return;
        len += tag_len;
        len += variant_uint64_size(field_value);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sint32_t>) {
        if (field_value.val == 0)
            return;
        len += tag_len;
        len += variant_uint64_size(ZigZagEncode32(field_value));
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sint64_t>) {
        if (field_value.val == 0)
            return;
        len += tag_len;
        len += variant_uint64_size(ZigZagEncode64(field_value));
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::fixed32_t>) {
        if (field_value.val == 0)
            return;
        len += tag_len;
        len += 4;
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::fixed64_t>) {
        if (field_value.val == 0)
            return;
        len += tag_len;
        len += 8;
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sfixed32_t>) {
        if (field_value.val == 0)
            return;
        len += tag_len;
        len += 4;
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sfixed64_t>) {
        if (field_value.val == 0)
            return;
        len += tag_len;
        len += 8;
    } else if constexpr (std::is_same_v<FieldType, bool>) {
        if (field_value == false)
            return;
        len += tag_len;
        len += 1;
    } else if constexpr (std::is_same_v<FieldType, float>) {
        if (field_value == 0.0f)
            return;
        len += tag_len;
        len += 4;
    } else if constexpr (std::is_same_v<FieldType, double>) {
        if (field_value == 0.0)
            return;
        len += tag_len;
        len += 8;
    } else if constexpr (std::is_same_v<FieldType, std::string> ||
                         std::is_same_v<FieldType, meegoo::pb::bytes>) {
        if (field_value.empty())
            return;
        len += tag_len;
        len += variant_uint32_size(field_value.size());
        len += field_value.size();
    } else if constexpr (meegoo::pb::is_sequence_container<FieldType>::value) {
        if (field_value.empty())
            return;

        size_t value_len = 0;
        const FieldType &value = field_value;
        for (size_t i = 0; i < value.size(); ++i) {
            calc_pb_size(trait, 0, 0, value[i], value_len);
        }
        len += tag_len;
        len += variant_uint32_size(value_len);
        len += value_len;
    } else if constexpr (meegoo::pb::is_associat_container<FieldType>::value) {
        for (const auto& [key, value] : field_value) {
            len += tag_len;

            size_t key_len = 0;
            meegoo::pb::calc_pb_size(trait, 1, 1, key, key_len);
            len += key_len;

            size_t value_len = 0;
            meegoo::pb::calc_pb_size(trait, 2, 1, value, value_len);
            len += value_len;

            len += variant_uint32_size(key_len + value_len);
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

        len += tag_len;
        len += variant_uint32_size(value_len);
        len += value_len;
    } else if constexpr (is_variant_v<FieldType>) {
        if (field_value.index() == trait.variant_index) {
            auto index = field_value.index();
            auto real_field_num = field_number + index;
            auto real_tag_len = calc_tag_size_constexpr(field_number << 3);

            size_t value_len = 0;
            std::visit([real_field_num, real_tag_len, &value_len, &trait](auto&& v){
                calc_pb_size(trait, real_field_num, real_tag_len, v, value_len);
            }, field_value);
            len += value_len;
        }
    }
    // if constexpr (!is_variant_v<FieldType>) {
    //     std::cout << "field_number: " << field_number
    //         << ", start pos: " << old_len
    //         << ", len: " << (len - old_len) << std::endl;
    // }
}

} // namespace meegoo::pb
