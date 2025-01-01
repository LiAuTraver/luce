#include <fmt/base.h>
#include <gtest/gtest.h>
#include <iostream>
#include <ranges>
#include <span>
#include <accat/auxilia/file_reader.hpp>
#include "MainMemory.hpp"
#include <cstdint>

TEST(load_program, bytes) {
  auto littleEndianData =
      *accat::auxilia::read_as_bytes<uint32_t>("Z:/luce/data/image.bin");

  auto little_endian_byte_span =
      std::span{reinterpret_cast<const std::byte *>(littleEndianData.data()),
                littleEndianData.size()};
  accat::luce::MainMemory<accat::luce::isa::instruction_set::riscv32> memory;
  memory.load_program(little_endian_byte_span);

  for (const auto i : std::ranges::views::iota(0ull, littleEndianData.size())) {
    EXPECT_EQ(*memory.read(0x80000000 + i), little_endian_byte_span[i]);
    fmt::println("r: {:#04x}, o: {:#04x}", static_cast<unsigned int>(*memory.read(0x80000000 + i)),
                 static_cast<unsigned int>(little_endian_byte_span[i]));
  }
}
