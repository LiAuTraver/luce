#pragma once

#include "accat/auxilia/details/program_options.hpp"
#include "accat/auxilia/details/Status.hpp"

namespace accat::luce {
auxilia::Status load_arguments();
auxilia::Status parse_arguments(const std::span<const std::string_view>);
inline auto &cli() {
  return auxilia::program_options::get("luce");
}
inline bool isBatchMode() {
  return auxilia::program_options::get("luce")
             .get_option("batch")
             ->value()
             .value_or("false") == "true";
}

} // namespace accat::luce
