#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace meegoo::pb {

class PayloadStream {
public:
    PayloadStream(uint8_t* data, size_t size)
        : data_(data), cur_(data), size_(size) {};

    uint8_t* data() const { return data_; }
    uint8_t* cur() const { return cur_; }
    bool set_cur(uint8_t* cur);
    uint8_t* seek(size_t pos);
    size_t cur_pos() const { return cur_ - data_; }
    size_t size() const { return size_; }
    size_t remain() const { return data_ + size_ - cur_;}

    uint8_t* write(const void* src, size_t size);
    uint8_t* read(size_t size);

    bool empty() const { return cur_ >= data_ + size_; }

    uint64_t decode_varint();
    uint32_t decode_fixed32();
    uint64_t decode_fixed64();

private:
    uint8_t* data_ = nullptr;
    uint8_t* cur_ = nullptr;
    size_t size_ = 0;
};

inline bool PayloadStream::set_cur(uint8_t* cur) {
    if (cur < data_ || cur > data_ + size_) {
        return false;
    }
    cur_ = cur;
    return true;
}

inline uint8_t* PayloadStream::seek(size_t pos) {
    if (pos > size_) {
        return 0;
    }
    cur_ = data_ + pos;
    return cur_;
}

inline uint8_t* PayloadStream::write(const void* src, size_t size) {
    if (cur_ + size > data_ + size_) {
        return nullptr;
    }
    uint8_t* ret = cur_;
    std::memcpy(cur_, src, size);
    cur_ += size;
    return ret;
}

inline uint8_t* PayloadStream::read(size_t size) {
    uint8_t* old = cur_;
    if (cur_ + size > data_ + size_) {
        return nullptr;
    }
    cur_ += size;
    return old;
}

inline uint64_t PayloadStream::decode_varint() {
    uint64_t val = 0;
    uint8_t* old = cur_;
    if (remain() > 0 && (*cur_ & 0x80) == 0) {
        val = *cur_;
        ++cur_;
    } else {
        size_t move_times = 0;
        while (true) {
            if (remain() == 0 && move_times >= 9) [[unlikely]] {
                throw std::runtime_error("invalid varint");
            }
            val |= uint64_t(*cur_ & 0x7f) << (move_times * 7);
            if ((*cur_ & 0x80) == 0) {
                ++cur_;
                break;
            }
            ++cur_;
            ++move_times;
        }
    }
    return val;
}

inline uint32_t PayloadStream::decode_fixed32() {
    if (remain() < 4) [[unlikely]] {
        throw std::runtime_error("invalid fix32");
    }
    uint32_t val = uint32_t(cur_[0]) | uint32_t(cur_[1]) << 8 |
        uint32_t(cur_[2]) << 16 | uint32_t(cur_[3]) << 24;
    cur_ += 4;
    return val;
}

inline uint64_t PayloadStream::decode_fixed64() {
    if (remain() < 8) [[unlikely]] {
        throw std::runtime_error("invalid fix32");
    }
    uint8_t val = uint64_t(cur_[0]) | uint64_t(cur_[1]) << 8 |
        uint64_t(cur_[2]) << 16 | uint64_t(cur_[3]) << 24 |
        uint64_t(cur_[4]) << 32 | uint64_t(cur_[5]) << 40 |
        uint64_t(cur_[6]) << 48 | uint64_t(cur_[7]) << 56;
    cur_ += 8;
    return val;
}

} // namespace meegoo::pb
