#pragma once

#include <meegoopb/define.h>
#include <meegoopb/impl/payload_stream.h>
#include <meegoopb/impl/trait.h>
#include <meegoopb/impl/calc_size.h>
#include <meegoopb/impl/reader.h>
#include <meegoopb/impl/writer.h>

namespace meegoo::pb {

template <typename T>
inline static size_t pb_size(const T &t) {
    size_t len = 0;
    refl_visit_members(t, [&len](const T& t, const auto &...args) {
        (std::visit([&t, &len](auto &arg) {
            calc_pb_size(arg, arg.field_number, arg.tag_len, t.*arg.offset, len);
        }, args), ...);
    });
    return len;
}

template <typename T>
inline static bool to_pb(const T &t, std::string &out) {
    PayloadStream stream(reinterpret_cast<uint8_t*>(out.data()), out.size());
    refl_visit_members(t, [&stream](const T& t, const auto &...args) {
        (std::visit([&t, &stream](auto &arg) {
            encode_pb_field(arg, arg.field_number, arg.tag_len, t.*arg.offset, stream);
        }, args), ...);
    });
    return true;
}

template <typename T>
inline static bool from_pb(T &t, const std::string &in) {
    PayloadStream stream(reinterpret_cast<uint8_t*>(const_cast<char*>(in.data())), in.size());
    decode_pb_field(t, stream);
    return true;
}

} // namespace meegoo::pb
