#pragma once

#include <filesystem>
#include <fstream>
#include <istream>
#include <ostream>
#include <sstream>
#include <utility>

#include "config.hpp"
#include "format.hpp"

namespace accat::auxilia {
/// @brief a simple file reader that reads the contents of a file
/// @note the file reader is not thread-safe, and will consume a lot of memory
/// if the file is too big.
class file_reader {
public:
  using path_t = path;
  using string_t = string;
  using ifstream_t = ifstream;
  using ostringstream_t = ostringstream;

public:
  inline explicit constexpr file_reader(path_t path_)
      : filePath(std::move(path_)) {}
  inline constexpr ~file_reader() = default;

public:
  [[nodiscard]] inline string_t get_contents() const {
    ifstream_t file(filePath);
    if (not file)
      return {};

    ostringstream_t buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }
  [[nodiscard]] inline path_t filepath() const { return filePath; }

private:
  const path_t filePath;
};
} // namespace accat::auxilia
