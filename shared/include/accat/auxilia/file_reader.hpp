#pragma once

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <sstream>
#include <ranges>
#include <string>
#include <bit>

#include "accat/auxilia/details/macros.hpp"
#include "config.hpp"
#include "Status.hpp"

namespace accat::auxilia {
namespace details {
template <typename TargetType>
Status check_file(const std::filesystem::path &path) noexcept {
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
} // namespace details
template <typename TargetType,
          std::endian Endianess = std::endian::native,
          typename CharType = char>
inline StatusOr<std::basic_string<CharType>>
read_as_bytes(const std::filesystem::path &path) noexcept {
  static_assert(Endianess == std::endian::native ||
                    Endianess == std::endian::big ||
                    Endianess == std::endian::little,
                "Unsupported endianess");

  if (auto res = details::check_file<TargetType>(path); !res)
    return {res};

  auto file = std::basic_ifstream<CharType>(path, std::ios::binary);
  auto buffer = std::basic_ostringstream<CharType>{};
  buffer << file.rdbuf();

  if constexpr (Endianess == std::endian::native)
    return {std::move(buffer).str()};

  // differs from the native endianess; reverse the bytes
  auto data = std::move(buffer).str();
  auto chunks = data | std::views::chunk(sizeof(TargetType));
  std::ranges::for_each(chunks, std::ranges::reverse);
  return {std::move(data)};
}
} // namespace accat::auxilia
