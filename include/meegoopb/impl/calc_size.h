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

template <typename T> constexpr std::string_view get_type_name() {
#ifdef __clang__
    return __PRETTY_FUNCTION__;
#elif defined(__GNUC__)
    return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
    return __FUNCSIG__;
#else
    return "Unknown compiler";
#endif
}

template <typename T> size_t calc_pb_varint_size(T field_value) { return 0; }

template <typename TraitType, typename ValueType, bool check_def = true>
inline static void calc_pb_size(TraitType trait, size_t field_number,
                                size_t tag_len, const ValueType &field_value,
                                size_t &len) {
    using FieldType = std::remove_const_t<std::remove_reference_t<ValueType>>;
    size_t old_len = len;
    if constexpr (std::is_same_v<FieldType, int32_t> ||
                  std::is_same_v<FieldType, int8_t> ||
                  std::is_same_v<FieldType, int16_t>) {
        if constexpr (check_def) {
            if (field_value == 0) [[unlikely]]
                return;
        }
        len += tag_len;
        len += variant_uint64_size(field_value);
    } else if constexpr (std::is_same_v<FieldType, int64_t> ||
                         std::is_same_v<FieldType, uint8_t> ||
                         std::is_same_v<FieldType, uint16_t>) {
        if constexpr (check_def) {
            if (field_value == 0) [[unlikely]]
                return;
        }
        len += tag_len;
        len += variant_uint64_size(field_value);
    } else if constexpr (std::is_same_v<FieldType, uint32_t>) {
        if constexpr (check_def) {
            if (field_value == 0) [[unlikely]]
                return;
        }
        len += tag_len;
        len += variant_uint32_size(field_value);
    } else if constexpr (std::is_same_v<FieldType, uint64_t>) {
        if constexpr (check_def) {
            if (field_value == 0) [[unlikely]]
                return;
        }
        len += tag_len;
        len += variant_uint64_size(field_value);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sint32_t>) {
        if constexpr (check_def) {
            if (field_value.val == 0) [[unlikely]]
                return;
        }
        len += tag_len;
        len += variant_uint64_size(ZigZagEncode32(field_value));
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sint64_t>) {
        if constexpr (check_def) {
            if (field_value.val == 0) [[unlikely]]
                return;
        }
        len += tag_len;
        len += variant_uint64_size(ZigZagEncode64(field_value));
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::fixed32_t>) {
        if constexpr (check_def) {
            if (field_value.val == 0) [[unlikely]]
                return;
        }
        len += (tag_len + 4);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::fixed64_t>) {
        if constexpr (check_def) {
            if (field_value.val == 0) [[unlikely]]
                return;
        }
        len += (tag_len + 8);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sfixed32_t>) {
        if constexpr (check_def) {
            if (field_value.val == 0) [[unlikely]]
                return;
        }
        len += (tag_len + 4);
    } else if constexpr (std::is_same_v<FieldType, meegoo::pb::sfixed64_t>) {
        if constexpr (check_def) {
            if (field_value.val == 0) [[unlikely]]
                return;
        }
        len += (tag_len + 8);
    } else if constexpr (std::is_same_v<FieldType, bool>) {
        if constexpr (check_def) {
            if (field_value == 0) [[unlikely]]
                return;
        }
        len += (tag_len + 1);
    } else if constexpr (std::is_same_v<FieldType, float>) {
        if constexpr (check_def) {
            if (field_value == 0.0f)
                return;
        }
        len += (tag_len + 4);
    } else if constexpr (std::is_same_v<FieldType, double>) {
        if constexpr (check_def) {
            if (field_value == 0.0)
                return;
        }
        len += (tag_len + 8);
    } else if constexpr (std::is_same_v<FieldType, std::string> ||
                         std::is_same_v<FieldType, meegoo::pb::bytes> ||
                         std::is_same_v<FieldType, std::vector<int8_t>> ||
                         std::is_same_v<FieldType, std::vector<uint8_t>> ||
                         meegoo::pb::is_byte_array_v<FieldType>) {
        if constexpr (check_def) {
            if (field_value.empty())
                return;
        }
        len += tag_len;
        if constexpr (meegoo::pb::is_byte_array_v<FieldType>) {
            constexpr size_t value_size = std::tuple_size_v<FieldType>;
            len += variant_uint32_size(field_value.size());
            len += value_size;
        } else {
            len += variant_uint32_size(field_value.size());
            len += field_value.size();
        }

    } else if constexpr (meegoo::pb::is_sequence_container<FieldType>::value ||
                         (meegoo::pb::is_array_v<FieldType> &&
                          !meegoo::pb::is_byte_array_v<FieldType>)) {
        if constexpr (check_def) {
            if (field_value.empty())
                return;
        }

        using ContainerValueType = typename FieldType::value_type;
        constexpr bool is_tlv = !std::is_arithmetic_v<ContainerValueType>;
        size_t count = field_value.size();
        if constexpr (is_tlv) { // 是否是 t-l-v 类型的
            size_t count = field_value.size();
            
            const FieldType &value = field_value;
            for (size_t i = 0; i < count; ++i) {
                size_t value_len = 0;
                calc_pb_size(trait, 0, 0, value[i], value_len);
                len += tag_len;
                len += value_len;
            }
        } else {
            size_t count = field_value.size();
            size_t value_len = 0;
            const FieldType &value = field_value;
            for (size_t i = 0; i < count; ++i) {
                calc_pb_size(trait, 0, 0, value[i], value_len);
            }
            len += tag_len;
            len += variant_uint32_size(value_len);
            len += value_len;                 
        }

    } else if constexpr (meegoo::pb::is_associat_container<FieldType>::value) {
        if constexpr (check_def) {
            if (field_value.empty())
                return;
        }
        for (const auto &[key, value] : field_value) {
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
        size_t value_len = [](auto &t) {
            size_t len = 0;
            refl_visit_tp_members(
                t, [&len](const auto &t, const auto &...args) {
                    (calc_pb_size(args, args.field_number, args.tag_len,
                                  t.*args.offset, len),
                     ...);
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
            std::visit(
                [real_field_num, real_tag_len, &value_len, &trait](auto &&v) {
                    calc_pb_size(trait, real_field_num, real_tag_len, v,
                                 value_len);
                },
                field_value);
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
