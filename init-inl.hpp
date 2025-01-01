#pragma once
#include <string_view>
#include <vector>
#include <span>
#include <ranges>

#include <accat/auxilia/memory.hpp>
#include "alter_args-inl.h"

namespace accat::luce {
[[nodiscard]] int luce_main([[maybe_unused]] std::span<const std::string_view>);

struct init {
private:
#ifdef AC_CPP_DEBUG
  inline static constexpr auto is_debug = true;
#else
  inline static constexpr auto is_debug = false;
#endif
public:
  using polymorphic_allocator_t = accat::auxilia::FixedSizeMemoryPool;
  inline AC_CONSTEXPR20 explicit init(int *argc, char ***argv) {
    if constexpr (is_debug) {
      if (!is_command_present(argc, argv, "-l", "--log")) {
        add_command(argc, argv, "--log=luce.log");
        return;
      }
    }
#if !defined(_MSC_VER) || defined(__clang__)
    auto stack_allocator = polymorphic_allocator_t::FromSize(
        (*argc + 1) * sizeof(std::string_view));
    args = std::pmr::vector<std::string_view>{&stack_allocator};
#endif
    args.reserve(*argc);
    for (const auto i : std::views::iota(0, *argc))
      args.emplace_back(*(*argv + i));
  }

#if defined(_MSC_VER) && !defined(__clang__)
  std::vector<std::string_view> args;
#else
  std::pmr::vector<std::string_view> args;
#endif

private:
  void add_command(int *&argc, char ***&argv, const std::string_view &command) {
#if !defined(_MSC_VER) || defined(__clang__)
    auto stack_allocator = polymorphic_allocator_t::FromSize(
        (*argc + 2) * sizeof(std::string_view));
    args = std::pmr::vector<std::string_view>{&stack_allocator};
#endif
    args.reserve(*argc + 1);
    for (const auto i : std::views::iota(0, *argc))
      args.emplace_back(*(*argv + i));
    args.emplace_back(command);
  }
};
} // namespace accat::luce
