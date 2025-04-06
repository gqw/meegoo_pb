#include <cstdint>
#include <gtest/gtest.h>
#include <meegoopb/impl/payload_stream.h>

// TEST(PayloadStream, TestNagative) {
//     {
//         std::vector<uint8_t> data{0b00001000, 0b01100100};
//         meegoo::pb::PayloadStream ps(const_cast<uint8_t*>(data.data()), data.size());
//         ASSERT_EQ(ps.decode_varint(), 0b00001000);
//     }
//     {
//         std::vector<uint8_t> data{0b10001000, 0b00000001};
//         meegoo::pb::PayloadStream ps(const_cast<uint8_t*>(data.data()), data.size());
//         ASSERT_EQ(ps.decode_varint(), 0b10001000);
//     }
//     {
//         std::vector<uint8_t> data{0b10011000, 0b10011000, 0b10011000, 0b10011000, 0b10011000,  0b10011000, 0b10011000, 0b10011000, 0b10011000, 0b00000001};
//         meegoo::pb::PayloadStream ps(const_cast<uint8_t*>(data.data()), data.size());
//         ASSERT_EQ(ps.decode_varint(), 0b1'0011000'0011000'0011000'0011000'0011000'0011000'0011000'0011000'0011000);
//     }

//     {
//         double d = 21.033;
//         uint64_t u64 = 0;
//         memcpy(&u64, &d, sizeof(double));
//         double d2;
//         memcpy(&d2, &u64, sizeof(double));
//         ASSERT_EQ(d, d2);
//     }
// }