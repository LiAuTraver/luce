#pragma once
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <accat/auxilia/auxilia.hpp>
#include <bit>
#include <cstddef>
#include <span>
#include <string>

#include "luce/config.hpp"
namespace accat::luce {
class Image : public auxilia::Printable {
public:
  Image() = default;
  Image(std::vector<std::byte> binary_data,
        std::endian endianess = std::endian::native)
      : is_little_endian_(endianess == std::endian::little ? true : false),
        binary_data_(std::move(binary_data)) {}
  ~Image() = default;

public:
  auto to_string(const auxilia::FormatPolicy &format_policy =
                     auxilia::FormatPolicy::kDefault) const -> string_type {
    return fmt::format(
        "[Image: size = {size}, endianess = {endianess}, bytes = {bytes}]",
        "size"_a = binary_data_.size(),
        "endianess"_a = is_little_endian_ ? "little" : "big",
        "bytes"_a = raw_string(format_policy));
  }
  auto raw_string(const auxilia::FormatPolicy format_policy =
                      auxilia::FormatPolicy::kDetailed) const -> string_type {
    auto isDetailed = format_policy == auxilia::FormatPolicy::kDetailed;
    auto isTrivialImage = binary_data_.size() < 20;

    return fmt::format("{rawStr:#04x}",
                       "rawStr"_a =
                           fmt::join(isTrivialImage || isDetailed
                                         ? binary_data_
                                         : std::span(binary_data_).first(20),
                                     " "));
  }
  bool is_little_endian() const noexcept {
    return is_little_endian_;
  }
  auto bytes_view() const noexcept -> std::span<const std::byte> {
    return binary_data_;
  }

private:
  bool is_little_endian_ = std::endian::native == std::endian::little;
  std::vector<std::byte> binary_data_;
};
} // namespace accat::luce
