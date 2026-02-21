#include "luce/args.hpp"

namespace accat::luce {
using Option = auxilia::program_options::Option;
auxilia::Status load_arguments() {
  auto &parser = cli();
  parser.add_option(Option("--image", "-i", "Image path"))
      .nargs(1)
      .default_value(R"(M:/Projects/luce/data/image.bin)");
  parser.add_option(Option("--batch", "-b", "Run in batch mode")).nargs(0);

  parser.add_option("--log", "-l", "Log file path").nargs(1);

  if (parser.error()) {
    return auxilia::InvalidArgumentError(
        fmt::format("Failed to load arguments: {}",
                    fmt::join(parser.error_messages(), "; ")));
  }
  return {};
}
auxilia::Status parse_arguments(const std::span<const std::string_view> args) {
  auto &parser = cli();
  parser.parse(args);
  if (parser.error()) {
    return auxilia::InvalidArgumentError(
        fmt::format("Failed to parse arguments: {}",
                    fmt::join(parser.error_messages(), "; ")));
  }
  return {};
}
} // namespace accat::luce
