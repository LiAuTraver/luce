

#include <argparse/argparse.hpp>
#include <iostream>
#include <span>
#include <string_view>
#include <vector>
using namespace std::literals;
struct ArgumentBase {
  std::pair<std::string_view, std::string_view> names;
  std::string_view help;
};
struct Flag {
  ArgumentBase _base_;

  constexpr auto operator->() const { return std::addressof(_base_); }
};
struct Single {
  ArgumentBase _base_;

  mutable std::string value{};

  constexpr auto operator->() const { return std::addressof(_base_); }
};
struct Multi {
  ArgumentBase _base_;

  mutable std::vector<std::string> values{};

  constexpr auto operator->() const { return std::addressof(_base_); }
};

static inline const auto log_ = Single{
    ._base_ = {.names = {"--log", "-l"}, .help = "Enable logging"sv},
};
static inline const auto batch = Flag{
    ._base_ =
        {.names = {"--batch", "-b"},
         .help =
             "Enable batch mode(run program instead of interactive shell)"sv},
};
static inline const auto image = Single{
    ._base_ = {.names = {"--image", "-i"}, .help = "Path to the image file"sv},
};
const auto args = std::vector<std::string_view>{
    "exe"sv,"--log=somefile.log"sv};

int main() {
  argparse::ArgumentParser program("demo");
  program.add_argument(log_->names.first, log_->names.second)
      .help(log_->help.data())
      .store_into(log_.value);

  try {
    program.parse_args({args.begin(), args.end()});
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    std::cerr << program;
    return 1;
  }
  std::cout << log_.value << std::endl;
}
