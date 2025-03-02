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
#include <memory>
#include <string>

#include "luce/Support/isa/IDisassembler.hpp"
#include "luce/Support/isa/architecture.hpp"
#include "luce/Support/isa/riscv32/Disassembler.hpp"
#include "luce/Support/isa/riscv32/instruction/Multiply.hpp"
#include "luce/Support/isa/riscv32/instruction/Atomic.hpp"
#include "luce/argument/Argument.hpp"

namespace accat::luce {
class ArgumentLoader;
}
namespace accat::luce {
class ExecutionContext : public auxilia::Printable {
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
  void initDisasm() {
    this->disassembler = std::make_unique<isa::Disassembler>();
    disassembler->initializeDefault()
        .addDecoder(std::make_unique<isa::instruction::multiply::Decoder>())
        .addDecoder(std::make_unique<isa::instruction::atomic::Decoder>());
  }

public:
  std::unique_ptr<isa::IDisassembler> disassembler;

public:
  static auto &InitializeContext(const ArgumentLoader &) {
    static bool has_initialized = false;
    static ExecutionContext ctx;
    if (has_initialized)
      return (ctx);

    defer {
      // just for testing purposes for argument parsing
      if (argument::program::testing.value) {
        spdlog::info("Testing mode enabled, exiting immediately");
        std::exit(EXIT_SUCCESS);
      }
    };
    ctx.initLog();
    ctx.initDisasm();
    return (ctx);
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

LUCE_API
[[nodiscard]] int main([[maybe_unused]] std::span<const std::string_view>);

} // namespace accat::luce
