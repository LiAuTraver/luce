#pragma once
#include <string_view>
#include <vector>
#include <span>

namespace accat::luce {
[[nodiscard]] int main(std::span<const std::string_view>);

// convert C-style args to span of string_views
[[nodiscard]] inline auto modernize_args(const int argc, char *const argv[]) {
  std::vector<std::string_view> args;
  args.reserve(argc);

  for (int i = 0; i < argc; ++i) {
    args.emplace_back(*(argv + i));
  }

#ifdef AC_CPP_DEBUG
  bool has_log = false;
  for (const auto arg : args) {
    if (arg == "-l" || arg.starts_with("--log")) {
      has_log = true;
      break;
    }
  }

  if (!has_log) {
    args.emplace_back("--log=luce.log");
  }
#endif

  return args;
}

} // namespace accat::luce
