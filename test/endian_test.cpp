#include "deps.hh"

#include <accat/auxilia/auxilia.hpp>
#include "luce/MainMemory.hpp"

#include <gtest/gtest.h>

TEST(EndianTest, Test) {
  auto littleEndianData =
      *accat::auxilia::read_as_bytes<uint32_t>("Z:/luce/data/image.bin");

  auto bigEndianData =
      *accat::auxilia::read_as_bytes<uint32_t, std::endian::big>(
          "Z:/luce/data/image-big_endian.bin");

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
