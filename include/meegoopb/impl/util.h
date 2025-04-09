#pragma once

#include <bitset>
#include <cstdint>
#include <iostream>
#include <iomanip>

#include <meegoopb/define.h>
#include <string_view>

namespace meegoo::pb {

constexpr inline uint32_t MakeTag(int field_number, WireType type) {
    return (static_cast<uint32_t>((static_cast<uint32_t>(field_number) << 3) |
                                  (type)));
}

template <typename T>
inline static uint8_t *UnsafeVarint(T value, uint8_t *ptr) {
    static_assert(std::is_unsigned<T>::value,
                  "Varint serialization must be unsigned");
    while (unlikely(value >= 0x80)) {
        *ptr = static_cast<uint8_t>(value | 0x80);
        value >>= 7;
        ++ptr;
    }
    *ptr++ = static_cast<uint8_t>(value);
    return ptr;
}

inline uint8_t *WriteVarint32ToArray(uint32_t value, uint8_t *target) {
    return UnsafeVarint(value, target);
}

inline uint8_t *WriteVarint64ToArray(uint64_t value, uint8_t *target) {
    return UnsafeVarint(value, target);
}

inline uint8_t *WriteTagToArray(int field_number, WireType type,
                                uint8_t *target) {
    if (field_number == 0) {
        return target;
    }
    return WriteVarint32ToArray(MakeTag(field_number, type), target);
}

inline uint32_t ZigZagEncode32(int32_t n) {
    // Note:  the right-shift must be arithmetic
    // Note:  left shift must be unsigned because of overflow
    return (static_cast<uint32_t>(n) << 1) ^ static_cast<uint32_t>(n >> 31);
}

inline int32_t ZigZagDecode32(uint32_t n) {
    // Note:  Using unsigned types prevent undefined behavior
    return static_cast<int32_t>((n >> 1) ^ (~(n & 1) + 1));
}

inline uint64_t ZigZagEncode64(int64_t n) {
    // Note:  the right-shift must be arithmetic
    // Note:  left shift must be unsigned because of overflow
    return (static_cast<uint64_t>(n) << 1) ^ static_cast<uint64_t>(n >> 63);
}

inline int64_t ZigZagDecode64(uint64_t n) {
    // Note:  Using unsigned types prevent undefined behavior
    return static_cast<int64_t>((n >> 1) ^ (~(n & 1) + 1));
}

inline uint8_t *WriteLittleEndian32ToArray(uint32_t value, uint8_t *target) {
    target[0] = static_cast<uint8_t>(value);
    target[1] = static_cast<uint8_t>(value >> 8);
    target[2] = static_cast<uint8_t>(value >> 16);
    target[3] = static_cast<uint8_t>(value >> 24);
    return target + sizeof(value);
}

inline uint8_t *WriteLittleEndianFloatToArray(float value, uint8_t *target) {
   
    target[0] = static_cast<uint8_t>(*((uint32_t*)&value));
    target[1] = static_cast<uint8_t>(*((uint32_t*)&value) >> 8);
    target[2] = static_cast<uint8_t>(*((uint32_t*)&value) >> 16);
    target[3] = static_cast<uint8_t>(*((uint32_t*)&value) >> 24);
    return target + sizeof(value);
}


inline uint8_t *WriteLittleEndian64ToArray(uint64_t value, uint8_t *target) {
    uint32_t part0 = static_cast<uint32_t>(value);
    uint32_t part1 = static_cast<uint32_t>(value >> 32);

    target[0] = static_cast<uint8_t>(part0);
    target[1] = static_cast<uint8_t>(part0 >> 8);
    target[2] = static_cast<uint8_t>(part0 >> 16);
    target[3] = static_cast<uint8_t>(part0 >> 24);
    target[4] = static_cast<uint8_t>(part1);
    target[5] = static_cast<uint8_t>(part1 >> 8);
    target[6] = static_cast<uint8_t>(part1 >> 16);
    target[7] = static_cast<uint8_t>(part1 >> 24);
    return target + sizeof(value);
}

inline uint8_t *WriteLittleEndianDoubleToArray(double value, uint8_t *target) {
    target[0] = static_cast<uint8_t>(*((uint64_t*)&value));
    target[1] = static_cast<uint8_t>(*((uint64_t*)&value) >> 8);
    target[2] = static_cast<uint8_t>(*((uint64_t*)&value) >> 16);
    target[3] = static_cast<uint8_t>(*((uint64_t*)&value) >> 24);
    target[4] = static_cast<uint8_t>(*((uint64_t*)&value) >> 32);
    target[5] = static_cast<uint8_t>(*((uint64_t*)&value) >> 40);
    target[6] = static_cast<uint8_t>(*((uint64_t*)&value) >> 48);
    target[7] = static_cast<uint8_t>(*((uint64_t*)&value) >> 56);
    return target + sizeof(value);
}


inline uint32_t log2_floor_uint32(uint32_t n) {
    return 31 ^ static_cast<uint32_t>(__builtin_clz(n));
}

inline size_t variant_uint32_size(uint64_t value) {
    uint32_t log2value = log2_floor_uint32(value | 0x1);
    return static_cast<size_t>((log2value * 9 + 73) / 64);
}


inline uint32_t log2_floor_uint64(uint64_t n) {
    return 63 ^ static_cast<uint32_t>(__builtin_clzll(n));
}

inline size_t variant_uint64_size(uint64_t value) {
    uint32_t log2value = log2_floor_uint64(value | 0x1);
    return static_cast<size_t>((log2value * 9 + 73) / 64);
}


inline void DebugPrint(std::string_view out, std::string_view name = "") {
    std::cout << "name: " << name.data() << " size: " << out.size() << std::endl;
    std::cout <<  std::right << std::setw(3) <<  0 << ": ";
    for (size_t i = 0; i < out.size(); i++) {
        std::bitset<8> bits(out[i]);
        std::cout << std::hex  << bits << " ";
        if (i % 10 == 9) {
            std::cout << std::endl;
            std::cout << std::right << std::setw(3)  << std::dec <<  i + 1 << ": ";
        }
    }
    std::cout << std::dec << std::endl;
}

} // namespace meegoo::pb
