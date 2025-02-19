#include "deps.hh"

#include <gtest/gtest.h>
#include <bit>

#include "luce/MainMemory.hpp"

using namespace accat::auxilia;

TEST(EndianTest, Test) {
  auto littleEndianData =
      *read_as_bytes<uint32_t, std::endian::little>("Z:/luce/data/image.bin");

  auto bigEndianData = *read_as_bytes<uint32_t, std::endian::big>(
      "Z:/luce/data/image-big-endian.bin");

  auto little_endian_byte_span =
      std::span{reinterpret_cast<const std::byte *>(littleEndianData.data()),
                littleEndianData.size()};
  auto big_endian_byte_span =
      std::span{reinterpret_cast<const std::byte *>(bigEndianData.data()),
                bigEndianData.size()};

  fmt::print("Data : {:#04x}\n", fmt::join(little_endian_byte_span, " "));
  fmt::print("Data2: {:#04x}\n", fmt::join(big_endian_byte_span, " "));

  EXPECT_EQ(littleEndianData, bigEndianData);
}
