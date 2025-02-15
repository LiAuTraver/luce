// a try of array inside union with alignas and mdspan
#include <array>
#include <cstddef>
#include <iostream>
#include <mdspan>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
static union alignas(4) {
  using R = std::array<std::byte, 4>;
  using RS = std::array<R, 32>;
  alignas(4) RS raw = {};
  alignas(4) struct {
    const R zero_reg = {std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0}};
    R ra;
    R sp;
    R gp;
    R tp;
    R t0;
    R t1;
    R t2;
    R s0;
    R s1;
    R a0;
    R a1;
    R a2;
    R a3;
    R a4;
    R a5;
    R a6;
    R a7;
    R s2;
    R s3;
    R s4;
    R s5;
    R s6;
    R s7;
    R s8;
    R s9;
    R s10;
    R s11;
    R t3;
    R t4;
    R t5;
    R t6;
  } registers;
};
size_t Str2Int(std::string_view str) {
  size_t result = 0;
  for (size_t i = 0; i < str.size(); ++i) {
    result = result * 10 + str[i] - '0';
  }
  return result;
}
struct Test {
  auto operator[](std::string_view x, size_t y, size_t z) {
    if (std::is_constant_evaluated()) {
      return threeDdata[Str2Int(x)][y][z];
    } else {
      throw std::runtime_error("Not a consteval function");
    }
  }
  auto operator[](int x, size_t y, size_t z) {
    if (std::is_constant_evaluated()) {
      return threeDdata[x][y][z];
    } else {
      throw std::runtime_error("Not a consteval function");
    }
  }
  std::array<std::array<std::array<unsigned char, 2>, 2>, 2> threeDdata = {
      1, 2, 3, 4, 5, 6, 7, 8}; // NOLINT
};
int test3() {
  // Create a 2D mdspan with (32 x 4) dynamic extents
  std::mdspan<std::byte, std::dextents<std::size_t, 2>> Registers(
      reinterpret_cast<std::byte *>(raw.data()), // pointer to data
      32,                                        // first dimension
      4                                          // second dimension
  );

  // assign some values from 0 to 127
  for (std::size_t i = 0; i < 32; ++i) {
    for (std::size_t j = 0; j < 4; ++j) {
      Registers[i, j] = static_cast<std::byte>(i * 4 + j);
    }
  }
  // print the values
  for (std::size_t i = 0; i < 32; ++i) {
    for (std::size_t j = 0; j < 4; ++j) {
      std::cout << std::hex << std::to_integer<int>(Registers[i, j]) << ' ';
    }
    std::cout << '\n';
  }
  // play with it
  std::cout << Registers.size() << '\n';
  std::cout << Registers.rank() << '\n';

  Test test;
  std::cout << test["1", 1, 1] << '\n';
  return 0;
}
int main() {
  std::string str = "    \tabcd   \t ref \n";
  auto placeholder = accat::auxilia::ranges::trim(str);
  std::cout << placeholder << '\n';
  return 0;
}