#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <type_traits>
#include <vector>
#include <deque>
#include <list>
#include <queue>
#include <map>
#include <variant>
#include <unordered_map>

#include "frozen/map.h"

namespace meegoo::pb {

constexpr size_t calc_tag_size_constexpr(uint32_t value) {
    if (value == 0) {
        return 1;
    }
    int log = 0;
    while (value >>= 1) ++log;
    return log / 7 + 1;
}


template <typename T> struct identity {};

template <typename T>
struct member_pointer_real_type;

// 偏特化：提取成员指针的类型
template <typename ClassType, typename MemberType>
struct member_pointer_real_type<MemberType ClassType::*> {
    using type = MemberType;
};

template <typename T>
using member_pointer_real_type_t = typename member_pointer_real_type<T>::type;


template <typename T>
struct field_trait_info {
    constexpr field_trait_info(T _offset,
        std::string_view _name, std::size_t _field_number)
        : offset(_offset), field_number(_field_number),
        name(_name) ,tag_len(calc_tag_size_constexpr(_field_number << 3)) {
            ;
        }
    constexpr std::size_t write_key() const { return field_number; }
    constexpr std::size_t read_key() const { return field_number; }
    T offset;
    std::size_t field_number;
    std::string_view name;
    size_t tag_len = 0;
};

template <typename T, size_t Index>
struct variant_field_trait_info {
    using variant_type = member_pointer_real_type_t<T>;
    using value_type = std::variant_alternative_t<Index, variant_type>;

    constexpr variant_field_trait_info(T _offset,
        std::string_view _name, std::size_t _field_number)
        : offset(_offset), field_number(_field_number),
        name(_name) ,tag_len(calc_tag_size_constexpr(_field_number << 3)),
        variant_index(Index){
    }
    constexpr std::size_t write_key() const { return field_number; }
    constexpr std::size_t read_key() const { return field_number + Index; }
    T offset;
    std::size_t field_number;
    std::string_view name;
    size_t tag_len = 0;
    size_t variant_index = 0;
};

#define variant_trait_info(member_ptr, name, field_number, index) \
    variant_field_trait_info<decltype(member_ptr), index>{member_ptr, name, field_number}

#define field_trait_info_v(var, name, ...) \
    varaint_trait_info<decltype(var), __VA_ARGS__>{var, name}

template<typename T>
[[maybe_unused]] inline constexpr decltype(auto) refl_offset_to_tuple();


template <typename T>
struct field_type_t;

template <typename... Args>
struct field_type_t<std::tuple<Args...>> {
  using value_type = std::variant<Args...>;
};

template <typename... Args>
struct field_type_t<std::variant<Args...>> {
  using value_type = std::tuple<Args...>;
};




template <typename T, size_t Size, typename Tuple, size_t... I>
constexpr auto inline get_write_field_map(Tuple& tp, std::index_sequence<I...>) {
  return frozen::map<uint32_t, T, sizeof...(I)>{
      {std::get<I>(tp).write_key(),
       T{std::in_place_index<I>, std::move(std::get<I>(tp))}}...};
}

template <typename T, size_t Size, typename Tuple, size_t... I>
constexpr auto inline get_read_field_map(Tuple& tp, std::index_sequence<I...>) {
  return frozen::map<uint32_t, T, sizeof...(I)>{
      {
        std::get<I>(tp).read_key(),
        T{std::in_place_index<I>, std::move(std::get<I>(tp))}
      }...};
}

template <typename TupleTrait>
inline static constexpr decltype(auto) write_tuple_to_map(TupleTrait tp_trait) {
    using tp_type = std::decay_t<decltype(tp_trait)>;
    using value_type = typename field_type_t<tp_type>::value_type;
    constexpr auto Size = std::tuple_size_v<tp_type>;
    return get_write_field_map<value_type, Size>(tp_trait, std::make_index_sequence<Size>{});
}

template <typename TupleTrait>
inline static constexpr decltype(auto) read_tuple_to_map(TupleTrait tp_trait) {
    using tp_type = std::decay_t<decltype(tp_trait)>;
    using value_type = typename field_type_t<tp_type>::value_type;
    constexpr auto Size = std::tuple_size_v<tp_type>;
    return get_read_field_map<value_type, Size>(tp_trait, std::make_index_sequence<Size>{});
}

template <typename T, typename Visitor, typename OrderedMap, std::size_t... Indices>
inline static constexpr decltype(auto) visit_map_impl(T& t, OrderedMap &&tp, Visitor &&visitor,
                                                        std::index_sequence<Indices...>) {
    // 展开索引序列，依次将 OrderedMap 的每个元素传递给 visitor
    return visitor(t, ((tp.begin() + Indices)->second)...);
}

template <typename T, typename Visitor, typename OrderedMap, std::size_t... Indices>
inline static constexpr decltype(auto) visit_tuple_impl(T& t, OrderedMap &&tp, Visitor &&visitor,
                                                        std::index_sequence<Indices...>) {
    // 展开索引序列，依次将 OrderedMap 的每个元素传递给 visitor
    return visitor(t, (std::get<Indices>(tp))...);
}

template <typename T, typename Visitor, typename OrderedMap, std::size_t... Indices>
inline static constexpr decltype(auto) visit_tuple_impl(const T& t, OrderedMap &&tp, Visitor &&visitor,
                                                        std::index_sequence<Indices...>) {
    // 展开索引序列，依次将 OrderedMap 的每个元素传递给 visitor
    return visitor(t, (std::get<Indices>(tp))...);
}

// template <typename T, typename Visitor, typename OrderedMap, std::size_t... Indices>
// inline static constexpr decltype(auto) visit_map_impl(const T& t, OrderedMap &&tp, Visitor &&visitor,
//                                                         std::index_sequence<Indices...>) {
//     // 展开索引序列，依次将 OrderedMap 的每个元素传递给 visitor
//     return visitor(t, ((tp.begin() + Indices)->second)...);
// }


template <typename T, typename Visitor>
inline static constexpr decltype(auto) refl_visit_tp_members(const T &t,
                                                          Visitor &&visitor) {
    constexpr auto tp_trait = refl_offset_to_tuple<T>();
    constexpr auto Size = std::tuple_size_v<decltype(tp_trait)>;

    return visit_tuple_impl(
        t, tp_trait, std::forward<Visitor>(visitor),
        std::make_index_sequence<Size>{});
}

template <typename T, typename Visitor>
inline static constexpr decltype(auto) refl_visit_tp_members(T &t,
                                                          Visitor &&visitor) {
    constexpr auto tp_trait = refl_offset_to_tuple<T>();
    constexpr auto Size = std::tuple_size_v<decltype(tp_trait)>;

    return visit_tuple_impl(
        t, tp_trait, std::forward<Visitor>(visitor),
        std::make_index_sequence<Size>{});
}

template <template <typename...> class U, typename T>
struct is_template_instant_of : std::false_type {};

template <template <typename...> class U, typename... args>
struct is_template_instant_of<U, U<args...>> : std::true_type {};

template <class T>
struct is_sequence_container
    : std::integral_constant<
          bool, is_template_instant_of<std::deque, T>::value ||
                    is_template_instant_of<std::list, T>::value ||
                    is_template_instant_of<std::vector, T>::value ||
                    is_template_instant_of<std::queue, T>::value> {};

template <class T>
struct is_associat_container
    : std::integral_constant<
            bool, is_template_instant_of<std::map, T>::value ||
                    is_template_instant_of<std::unordered_map, T>::value> {};
                    
// 定义一个检测模板
template <typename, typename = std::void_t<>>
struct is_refl_struct : std::false_type {};

// 假设我们要检测某个类型是否有名为 `foo` 的成员函数
template <typename T>
struct is_refl_struct<T, std::void_t<decltype(refl_offset_to_tuple<T>())>> : std::true_type {};

// 辅助变量模板（C++17）
template <typename T>
constexpr bool is_refl_struct_v = is_refl_struct<T>::value;

template <typename T>
struct is_variant : std::false_type {};

template <typename... T>
struct is_variant<std::variant<T...>> : std::true_type {};

template <typename T>
constexpr inline bool is_variant_v = is_variant<T>::value;

template <typename T>
struct is_array : std::false_type {};

template <typename T, std::size_t N>
struct is_array<std::array<T, N>> : std::true_type {};

template <typename T>
constexpr inline bool is_array_v = is_array<T>::value;

template <typename T>
struct is_byte_array : std::false_type {};

// 针对 std::array<T, N> 的特化
template <typename T, std::size_t N>
struct is_byte_array<std::array<T, N>> : std::integral_constant<bool, (sizeof(T) == 1)> {};

// 提供一个变量模板，方便使用
template <typename T>
constexpr inline bool is_byte_array_v = is_byte_array<T>::value;
}  // namespace meegoo::pb