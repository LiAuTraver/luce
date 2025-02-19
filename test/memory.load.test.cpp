#include "deps.hh"

#include <gtest/gtest.h>

#include "luce/Support/isa/architecture.hpp"
#include "luce/MainMemory.hpp"

using namespace accat::luce;
using namespace accat::auxilia;

TEST(load_program, bytes) {
  auto littleEndianData = *read_as_bytes<uint32_t>("Z:/luce/data/image.bin");

  auto little_endian_byte_span =
      std::span{reinterpret_cast<const std::byte *>(littleEndianData.data()),
                littleEndianData.size()};
  MainMemory memory{nullptr};
  memory.load_program(
      little_endian_byte_span, isa::virtual_base_address, 0x10000);

  std::vector<std::byte> readData;
  readData.reserve(littleEndianData.size());
  for (const auto i : std::views::iota(0ull, littleEndianData.size())) {
    readData.push_back(*memory.read(isa::physical_base_address + i));
    EXPECT_EQ(*memory.read(isa::physical_base_address + i),
              little_endian_byte_span[i]);
  }
  fmt::println("Orig: {:#04x}\n"
               "Read: {:#04x}",
               fmt::join(littleEndianData, " "),
               fmt::join(readData, " "));
}
