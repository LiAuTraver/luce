#include "deps.hh"

#include "luce/Argument.hpp"

#include "luce/ArgumentLoader.hpp"

#include <forward_list>
#include <argparse/argparse.hpp>
#include <span>

namespace accat::luce::argument {
namespace program {
Flag batch = {{"--batch", "-b"},
              "Enable batch mode(run program instead of interactive shell)"};
Flag testing = {{"--testing", "-t"},
                "Enable testing mode(nothing but exit immediately)"};
Single log = {{"--log", "-l"}, "Enable logging"};
Single image = {{"--image", "-i"}, "Path to the image file"};
std::span<Argument *> args() {
  static Argument *args_array[] = {&batch, &testing, &log, &image};
  return {args_array};
}
} // namespace program
// namespace repl {
// Flag continue_repl = {{"--continue-repl", "c"},
//                       "Continue the REPL after running the program"};
// Flag exit = {{"--exit", "q"}, "Exit the REPL after running the program"};
// Single step = {{"--step", "si"}, "Step through the program", "1"};
// } // namespace repl

void Flag::add_to_parser(ArgumentLoader &parser) {
  parser->add_argument(names.first, names.second)
      .help(help)
      .flag()
      .store_into(value);
}
void Single::add_to_parser(ArgumentLoader &parser) {
  auto my_arg = &parser->add_argument(names.first, names.second)
                     .help(help)
                     .nargs(1)
                     .store_into(value);
  if (!value.empty()) {
    my_arg->default_value(value);
  }
}
void Multi::add_to_parser(ArgumentLoader &parser) {
  auto my_arg = &parser->add_argument(names.first, names.second)
                     .help(help)
                     .nargs(values.size())
                     .store_into(values);
  if (!values.empty()) {
    my_arg->default_value(values);
  }
}
} // namespace accat::luce::argument
