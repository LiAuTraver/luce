#include "accat/auxilia/details/Status.hpp"
#include "deps.hh"

#include "luce/ArgumentLoader.hpp"
#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>
#include <iostream>
#include <type_traits>
namespace accat::luce::argument {
Flag batch = {
    {.names = {"--batch", "-b"},
     .help = "Enable batch mode(run program instead of interactive shell)"},
};
// ??? can inline-ed???
inline Single log = {
    {.names = {"--log", "-l"}, .help = "Enable logging"},
};
Single image = {
    {.names = {"--image", "-i"}, .help = "Path to the image file"},
};
} // namespace accat::luce::argument
namespace accat::luce::detail {
using ap = argparse::ArgumentParser;
using namespace argument;
using auxilia::match;
namespace {
inline void _do_add_flag(ap &, Flag &);
inline void _do_add_single(ap &, Single &);
inline void _do_add_multi(ap &, Multi &);
void _add_single_argument(ap &, const auto &);
} // namespace
} // namespace accat::luce::detail
namespace accat::luce {
ArgumentLoader::ArgumentLoader(const std::string_view programName,
                               const std::string_view programVersion)
    : program(new argparse::ArgumentParser{programName.data(),
                                           programVersion.data()}) {}
ArgumentLoader::ArgumentLoader(ArgumentLoader &&that) noexcept {
  this->operator=(std::move(that));
}
ArgumentLoader &ArgumentLoader::operator=(ArgumentLoader &&that) noexcept {
  if (this != &that) {
    this->program->argparse::ArgumentParser::~ArgumentParser();
    this->program = that.program;
    that.program = nullptr;
  }
  return *this;
}
ArgumentLoader &ArgumentLoader::load_arguments() {
  using detail::_add_single_argument;
  auto &parser = *this->program;

  _add_single_argument(parser, argument::log);
  _add_single_argument(parser, argument::batch);
  _add_single_argument(parser, argument::image);

  return *this;
}
auxilia::Status
ArgumentLoader::parse_arguments(const std::span<const std::string_view> args) {
  try {
    program->parse_args({args.begin(), args.end()});
    return auxilia::OkStatus();
  } catch (const std::exception &e) {
    return auxilia::InvalidArgumentError(e.what());
  }
}
ArgumentLoader::~ArgumentLoader() { delete program; }
} // namespace accat::luce
namespace accat::luce::detail {
namespace {
void _do_add_flag(ap &parser, Flag &f) {
  parser.add_argument(f.names.first, f.names.second).help(f.help).flag();
}
void _do_add_single(ap &parser, Single &s) {
  auto my_arg = &parser.add_argument(s.names.first, s.names.second)
                     .help(s.help)
                     .nargs(1)
                     .store_into(s.value);
  if (!s.value.empty()) {
    my_arg->default_value(s.value);
  }
}
void _do_add_multi(ap &parser, Multi &m) {
  auto my_arg = &parser.add_argument(m.names.first, m.names.second)
                     .help(m.help)
                     .nargs(m.values.size())
                     .store_into(m.values);
  if (!m.values.empty()) {
    my_arg->default_value(m.values);
  }
}
void _add_single_argument(argparse::ArgumentParser &parser, const auto &_arg_) {
  match{[&](const argument::Flag &f) {
          _do_add_flag(parser, const_cast<argument::Flag &>(f));
        },
        [&](const argument::Single &s) {
          _do_add_single(parser, const_cast<argument::Single &>(s));
        },
        [&](const argument::Multi &m) {
          _do_add_multi(parser, const_cast<argument::Multi &>(m));
        },
        [&](const auto &_whatever_) {
          spdlog::error("Unknown argument type: {}", typeid(_whatever_).name());
          dbg_break
        }}(_arg_);
}
} // namespace
} // namespace accat::luce::detail
