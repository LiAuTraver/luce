#pragma once
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <luce/config.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <accat/auxilia/auxilia.hpp>
#include <luce/isa/architecture.hpp>
#include <string>
#include "luce/ArgumentLoader.hpp"

namespace accat::luce {
class ExecutionContext : public auxilia::Printable<ExecutionContext> {
public:
  static auto &InitializeContext(const ArgumentLoader &) {
    static ExecutionContext ctx;
    AC_SPDLOG_INITIALIZATION(luce, debug)
    if (auto logFile = argument::log.value; !logFile.empty()) {
      spdlog::info("log file: {}", std::filesystem::absolute(logFile));
      auto console_sink =
          std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
          std::move(logFile), true);
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
  [[nodiscard]] auto to_string(
      const auxilia::FormatPolicy & = auxilia::FormatPolicy::kDefault) const
      -> std::string {
    return "ExecutionContext";
  }
};

LUCE_API [[nodiscard]] int
    luce_main([[maybe_unused]] std::span<const std::string_view>);

} // namespace accat::luce
