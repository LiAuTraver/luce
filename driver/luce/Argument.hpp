#pragma once
#include <string>
#include <vector>
#include <utility>

namespace accat::luce::argument {
using namespace std::literals;
struct Argument {
  const std::pair<const char*, const char*> names;
  const char* help;
};
struct Flag : Argument {};
struct Single : Argument {
  mutable std::string value{};
};
struct Multi : Argument {
  mutable std::vector<std::string> values{};
};
extern Flag batch;
extern Single log;
extern Single image;

} // namespace accat::luce::argument
