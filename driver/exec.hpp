#pragma once
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <luce/config.hpp>
#include <spdlog/common.h>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <accat/auxilia/auxilia.hpp>
#include <luce/isa/architecture.hpp>
#include <string>
#include "luce/argument/Argument.hpp"

namespace accat::luce {
class ArgumentLoader;
}
namespace accat::luce {
class ExecutionContext : public auxilia::Printable<ExecutionContext> {
public:
  void initLog() {
    AC_SPDLOG_INITIALIZATION(luce, debug)

    auto logFile = argument::program::log.value;
    if (logFile.empty()) {
      dbg(info, "no log file, only write to stdout")
      return;
    }
    spdlog::info("log file: {}", std::filesystem::absolute(logFile));
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
        std::move(logFile), false); // 'false' to append the log file
    file_sink->set_level(spdlog::level::trace);
    console_sink->set_level(spdlog::level::debug);
    console_sink->set_pattern("[%^%l%$] %v");

    {
      auto temp_logger = spdlog::logger{"luce_loader", {file_sink}};
      temp_logger.info("\n\n\n===--------------------LOG "
                       "STARTING---------------------===");
    }

    spdlog::set_default_logger(std::make_shared<spdlog::logger>(
        spdlog::logger{"luce", {console_sink, file_sink}}));
    return;
  }
  static auto &InitializeContext(const ArgumentLoader &) {
    static ExecutionContext ctx;
    ctx.initLog();
    defer {
      // just for testing purposes for argument parsing
      if (argument::program::testing.value) {
        spdlog::info("Testing mode enabled, exiting immediately");
        std::exit(EXIT_SUCCESS);
      }
    };
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
    main([[maybe_unused]] std::span<const std::string_view>);

} // namespace accat::luce
