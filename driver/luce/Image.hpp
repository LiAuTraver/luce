#pragma once
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <accat/auxilia/auxilia.hpp>
#include <accat/auxilia/details/format.hpp>
#include <bit>
#include <cstddef>
#include <span>
#include <string>
namespace accat::luce {
class Image : public auxilia::Printable<Image> {
public:
  Image() = default;
  Image(std::vector<std::byte> binary_data,
        std::endian endianess = std::endian::native)
      : binary_data_(std::move(binary_data)),
        is_little_endian_(endianess == std::endian::little ? true : false) {}
  ~Image() = default;

public:
  auto to_string(const auxilia::FormatPolicy &format_policy =
                     auxilia::FormatPolicy::kDefault) const -> string_type {
    return fmt::format("[Image: size = {}, endianess = {}, bytes = {}]",
                       binary_data_.size(),
                       is_little_endian_ ? "little" : "big",
                       raw_string(format_policy));
  }
  string_type raw_string(const auxilia::FormatPolicy format_policy =
                             auxilia::FormatPolicy::kDetailed) const {
    auto isDetailed = format_policy == auxilia::FormatPolicy::kDetailed;
    auto isTrivialImage = binary_data_.size() < 20;

    return fmt::format("{:#04x}",
                       fmt::join(isTrivialImage ? binary_data_
                                 : isDetailed
                                     ? binary_data_
                                     : std::span(binary_data_).first(20),
                                 " "));
  }
  bool is_little_endian() const noexcept { return is_little_endian_; }
  auto bytes_view() const noexcept -> std::span<const std::byte> {
    return binary_data_;
  }

private:
  bool is_little_endian_ = std::endian::native == std::endian::little;
  std::vector<std::byte> binary_data_;
};
class ImageView : public auxilia::Printable<ImageView> {};
} // namespace accat::luce
