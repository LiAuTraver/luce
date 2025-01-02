#include <gtest/gtest.h>
#include <iostream>
#include <ranges>
#include <span>
#include <accat/auxilia/auxilia.hpp>
#include <cstdint>
#include <luce/MainMemory.hpp>
import std;
using namespace accat::luce;
TEST(load_program, bytes) {
  auto littleEndianData =
      *accat::auxilia::read_as_bytes<uint32_t>("Z:/luce/data/image.bin");

  auto little_endian_byte_span =
      std::span{reinterpret_cast<const std::byte *>(littleEndianData.data()),
                littleEndianData.size()};
  MainMemory<isa::instruction_set::riscv32> memory;
  memory.load_program(little_endian_byte_span);

  std::vector<std::byte> readData;
  readData.reserve(littleEndianData.size());
  for (const auto i : std::ranges::views::iota(0ull, littleEndianData.size())) {
    readData.push_back(*memory.read(0x80000000 + i));
    EXPECT_EQ(*memory.read(0x80000000 + i), little_endian_byte_span[i]);
  }
  fmt::println("Orig: {:#04x}\n"
               "Read: {:#04x}",
               fmt::join(littleEndianData, " "),
               fmt::join(readData, " "));
}
