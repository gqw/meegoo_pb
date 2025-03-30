#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <map>

namespace meegoo::pb {

#if defined __clang__
#define PB_INLINE __attribute__((always_inline)) inline
#define PB__INLINE_LAMBDA __attribute__((always_inline)) constexpr
#elif defined _MSC_VER
#define PB_INLINE __forceinline
#define PB__INLINE_LAMBDA constexpr
#else
#define PB_INLINE __attribute__((always_inline)) inline
#define PB__INLINE_LAMBDA constexpr __attribute__((always_inline))
#endif

#ifdef __has_builtin
#define HAVE_BUILTIN(x) __has_builtin(x)
#else
#define HAVE_BUILTIN(x) 0
#endif

#if HAVE_BUILTIN(__builtin_expect) || \
    (defined(__GNUC__) && !defined(__clang__))
#define unlikely(x) (__builtin_expect(false || (x), false))
#define likely(x) (__builtin_expect(false || (x), true))
#else
#define unlikely(x) (x)
#define likely(x) (x)
#endif

enum WireType : int {
    WIRETYPE_VARINT = 0,
    WIRETYPE_FIXED64 = 1,
    WIRETYPE_LENGTH_DELIMITED = 2,
    WIRETYPE_START_GROUP = 3,
    WIRETYPE_END_GROUP = 4,
    WIRETYPE_FIXED32 = 5,
};

struct sint32_t {
    using value_type = int32_t;
    int32_t val;
    sint32_t(int32_t v) { val = v; }
    operator int32_t() const { return val; }
    bool operator==(const sint32_t& other) const { return val == other.val; }
};

struct sint64_t {
    using value_type = int64_t;
    int64_t val;
    sint64_t(int64_t v) { val = v; }
    operator int64_t() const { return val; }
    bool operator==(const sint64_t& other) const { return val == other.val; }
};

struct fixed32_t {
    using value_type = uint32_t;
    uint32_t val;
    fixed32_t(uint32_t v) { val = v; }
    operator uint32_t() const { return val; }
    bool operator==(const fixed32_t& other) const { return val == other.val; }
};

struct fixed64_t {
    using value_type = uint64_t;
    uint64_t val;
    fixed64_t(uint64_t v) { val = v; }
    operator uint64_t() const { return val; }
    bool operator==(const fixed64_t& other) const { return val == other.val; }
};

struct sfixed32_t {
    using value_type = int32_t;
    int32_t val;
    sfixed32_t(int32_t v) { val = v; }
    operator int32_t() const { return val; }
    bool operator==(const sfixed32_t& other) const { return val == other.val; }
};

struct sfixed64_t {
    using value_type = int64_t;
    int64_t val;
    sfixed64_t(int64_t v) { val = v; }
    operator int64_t() const { return val; }
    bool operator==(const sfixed64_t& other) const { return val == other.val; }
};

using bytes = std::vector<uint8_t>;

template<typename ValueType>
using google_any = std::map<std::string, ValueType>;


} // namespace meegoo::pb