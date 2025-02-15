#include "deps.hh"

#include <spdlog/spdlog.h>
#include <argparse/argparse.hpp>
#include <iostream>
#include <type_traits>

#include "luce/argument/ArgumentLoader.hpp"

namespace accat::luce::detail {
using ap = argparse::ArgumentParser;
using namespace argument;
using auxilia::match;
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
ArgumentLoader &
ArgumentLoader::load_arguments(const std::span<argument::Argument *> args) {
  std::ranges::for_each(args, [this](auto arg) { arg->add_to_parser(*this); });
  return *this;
}
auxilia::Status
ArgumentLoader::parse_arguments(const std::span<const std::string_view> args) {
  try {
    program->parse_args({args.begin(), args.end()});
    return {};
  } catch (const std::exception &e) {
    return auxilia::InvalidArgumentError(e.what());
  }
}
ArgumentLoader::~ArgumentLoader() {
  delete program;
}
} // namespace accat::luce
