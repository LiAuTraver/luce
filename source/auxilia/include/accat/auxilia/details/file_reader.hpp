#pragma once

#include "./Status.hpp"
#include "./config.hpp"
#include <algorithm>
#include <bit>
#include <cstddef>
#include <vector>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <future>

namespace accat::auxilia::details {
template <typename TargetType>
inline Status check_file(const std::filesystem::path &path) noexcept {
  if (not std::filesystem::exists(path))
    return NotFoundError("The file does not exist");

  // if (std::filesystem::file_size(path) % sizeof(TargetType))
  // ^^^^ this THROWs! ^^^^ we want `noexcept`

  auto ec = std::error_code{};
  auto fileSize = std::filesystem::file_size(path, ec);
  if (ec)
    return UnknownError(ec.message());
  if (fileSize % sizeof(TargetType))
    return InvalidArgumentError(
        "The file size is not a multiple of the target type size");

  return OkStatus();
}
} // namespace accat::auxilia::details
EXPORT_AUXILIA
namespace accat::auxilia {
template <typename TargetType, std::endian Endianess = std::endian::native,
          typename CharType = char>
inline StatusOr<std::basic_string<CharType>>
read_as_bytes(const std::filesystem::path &path) {
  static_assert(Endianess == std::endian::native ||
                    Endianess == std::endian::big ||
                    Endianess == std::endian::little,
                "Unsupported endianess");

  if (auto res = details::check_file<TargetType>(path); !res)
    return {res};

  auto file =
      std::basic_ifstream<CharType>(path, std::ios::in | std::ios::binary);
  auto buffer = std::basic_ostringstream<CharType>{};
  buffer << file.rdbuf();

  if constexpr (Endianess == std::endian::native)
    return {std::move(buffer.str())};

  // differs from the native endianess; reverse the bytes
  auto data = std::move(buffer).str();
  std::ranges::for_each(data | std::views::chunk(sizeof(TargetType)),
                        std::ranges::reverse);
  return {std::move(data)};
}
template <typename CharType = char>
std::vector<std::byte> as_raw_bytes(const std::basic_string<CharType> &data) {
  // clang-format off
  return data 
          | std::views::transform([](auto &&c) {
              return static_cast<std::byte>(c);
            })
          | std::views::common
          | std::ranges::to<std::vector<std::byte>>();
  // clang-format on
}
// read raw bytes
template <std::endian Endianess = std::endian::native>
inline StatusOr<std::vector<std::byte>>
read_raw_bytes(const std::filesystem::path &path) {
  if (auto res = read_as_bytes<char, Endianess, char>(path); !res) {
    return {res};
  } else {
    return as_raw_bytes(*res);
  }
}
auto async(auto &&func, auto... args) -> decltype(auto) {
  return std::async(std::launch::async, std::forward<decltype(func)>(func),
                    std::forward<decltype(args)>(args)...);
}
} // namespace accat::auxilia
