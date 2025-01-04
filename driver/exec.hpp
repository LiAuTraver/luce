#pragma once
#include <filesystem>
#include <luce/config.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <accat/auxilia/auxilia.hpp>
#include <luce/isa/architecture.hpp>

namespace accat::luce {
class ExecutionContext : public auxilia::Printable<ExecutionContext> {
public:
  // using host_arch_t = isa::Architecture<isa::instruction_set::host>;
  using host_address_t = typename isa::host::virtual_address_t;
  // using guest_arch_t = isa::Architecture;
  using guest_address_t = typename isa::virtual_address_t;

public:
  static auto &InitializeContext(const std::span<const std::string_view> args) {
    auto argc = args.size();
    static ExecutionContext ctx;
    // TODO: parse args
    // ...
    AC_SPDLOG_INITIALIZATION(emu, debug)
    bool has_log_file = false;
    std::string log_file;

    if (auto iter = std::ranges::find_if(args,
                                         [](const auto &arg) {
                                           return arg.starts_with("-l") ||
                                                  arg.starts_with("--log");
                                         });
        iter != args.end()) {
      // -log/-l=luce.log
      has_log_file = true;
      if (iter->find('=') != std::string_view::npos) {
        log_file = iter->substr(iter->find('=') + 1);
      } else {
        log_file = std::string{iter->substr(4)};
      }
    }

    if (has_log_file) {
      dbg(info, "log file: {}", std::filesystem::absolute(log_file))
      auto console_sink =
          std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
          std::move(log_file), true);
      file_sink->set_level(spdlog::level::trace);
      console_sink->set_level(spdlog::level::debug);
      console_sink->set_pattern("[%^%l%$] %v");
      spdlog::set_default_logger(std::make_shared<spdlog::logger>(
          spdlog::logger{"luce", {console_sink, file_sink}}));
    } else {
      dbg(info, "no log file, only write to stdout")
    }
    return ctx;
  }

  ExecutionContext(const ExecutionContext &) = delete;
  auto operator=(const ExecutionContext &) = delete;

protected:
  inline explicit constexpr ExecutionContext() = default;

  inline ~ExecutionContext() = default;

public:
  [[nodiscard]] auto to_string(const auxilia::FormatPolicy &format_policy =
                                   auxilia::FormatPolicy::kDefault) const
      -> std::string {
    return "ExecutionContext";
  }
};

LUCE_API [[nodiscard]] int
    luce_main([[maybe_unused]] std::span<const std::string_view>);

} // namespace accat::luce
