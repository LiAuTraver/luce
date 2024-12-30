#pragma once
#include <cstdint>
#include <span>

#include <accat/auxilia/format.hpp>

#include "isa/architecture.hpp"
namespace accat::luce {

template <isa::instruction_set ISA>
class ExecutionContext
    : public auxilia::Printable<ExecutionContext<ISA>> {
  friend class auxilia::Printable<ExecutionContext<ISA>>;

public:
  using host_arch_t = isa::Architecture<isa::instruction_set::host>;
  using host_address_t = typename host_arch_t::physical_address_t;
  using guest_arch_t = isa::Architecture<ISA>;
  using guest_address_t = typename guest_arch_t::physical_address_t;

public:
  static auto &
  InitializeContext(const std::span<const std::string_view> args) {
    auto argc = args.size();
    static ExecutionContext<ISA> ctx;
    // TODO: parse args
    // ...
    
    return ctx;
  }

  ExecutionContext(const ExecutionContext &) = delete;
  auto operator=(const ExecutionContext &) = delete;

protected:
  inline explicit constexpr ExecutionContext() = default;

  inline ~ExecutionContext() = default;

private:
  [[nodiscard]] auto
  to_string_impl(const auxilia::FormatPolicy &format_policy) const
      -> std::string {
    return "ExecutionContext";
  }
};
} // namespace accat::luce
