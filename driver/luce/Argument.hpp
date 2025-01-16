#pragma once
#include <string>
#include <utility>
#include <vector>
#include <utility>

namespace accat::luce {
class ArgumentLoader;
}
namespace accat::luce::argument {
using namespace std::literals;
struct Argument {
  const std::pair<const char *, const char *> names{nullptr, nullptr};
  const char *help = nullptr;

  virtual ~Argument() = default;
  virtual void add_to_parser(ArgumentLoader &) = 0;

  Argument() = default;
  Argument(const Argument &) = delete;
  Argument &operator=(const Argument &) = delete;
  Argument(const std::pair<const char *, const char *> names, const char *help)
      : names(names), help(help) {}
};
struct Flag : Argument {
  bool value{false};
  virtual void add_to_parser(ArgumentLoader &) override;

  Flag() = default;
  Flag(const Flag &) = delete;
  Flag &operator=(const Flag &) = delete;
  Flag(const std::pair<const char *, const char *> names,
       const char *help,
       const bool value = false)
      : Argument(names, help), value(value) {}
};
struct Single : Argument {
  std::string value{};
  virtual void add_to_parser(ArgumentLoader &) override;

  Single() = default;
  Single(const Single &) = delete;
  Single &operator=(const Single &) = delete;
  Single(const std::pair<const char *, const char *> names,
         const char *help,
         std::string value = "")
      : Argument(names, help), value(std::move(value)) {}
};
struct Multi : Argument {
  std::vector<std::string> values{};
  virtual void add_to_parser(ArgumentLoader &) override;

  Multi() = default;
  Multi(const Multi &) = delete;
  Multi &operator=(const Multi &) = delete;
  Multi(const std::pair<const char *, const char *> names,
        const char *help,
        std::vector<std::string> values = {})
      : Argument(names, help), values(std::move(values)) {}
};
namespace program {
extern Flag testing;
extern Flag batch;
extern Single log;
extern Single image;
extern std::span<Argument *> args();
} // namespace program
} // namespace accat::luce::argument
