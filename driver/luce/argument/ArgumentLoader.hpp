#pragma once

#include <string_view>
#include <vector>
#include <span>

#include <accat/auxilia/auxilia.hpp>

#include "Argument.hpp"

/// @brief forward declaration, reduce compile time
namespace argparse {
class ArgumentParser;
}
namespace accat::luce {
class ArgumentLoader {
public:
  explicit ArgumentLoader(std::string_view, std::string_view);
  ArgumentLoader(const ArgumentLoader &) = delete;
  ArgumentLoader &operator=(const ArgumentLoader &) = delete;
  ArgumentLoader(ArgumentLoader &&) noexcept;
  ArgumentLoader &operator=(ArgumentLoader &&) noexcept;
  ~ArgumentLoader();

public:
  ArgumentLoader &load_arguments(std::span<argument::Argument *>);
  auxilia::Status parse_arguments(std::span<const std::string_view>);
  auto get(this auto &&self, auto &&...args) {
    return self.program->template get<decltype(args)...>(
        std::forward<decltype(args)>(args)...);
  }
  auto operator->() noexcept {
    precondition(program, "program is nullptr");
    return program;
  }

private:
  argparse::ArgumentParser *program = nullptr;
};
} // namespace accat::luce
