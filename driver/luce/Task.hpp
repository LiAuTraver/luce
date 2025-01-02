#pragma once

#include <spdlog/spdlog.h>
#include <accat/auxilia/auxilia.hpp>
import std;

#include "isa/architecture.hpp"


namespace accat::luce {
enum class Permission : uint8_t {
  kNone = 0,
  kRead = 1,
  kWrite = 2,
  kExecute = 4,
  kReadWrite = kRead | kWrite,
  kReadExecute = kRead | kExecute,
  kReadWriteExecute = kRead | kWrite | kExecute,
};
AC_BITMASK_OPS(Permission);
template <isa::instruction_set ISA> class AddressSpace {
  using virtual_address_t = typename isa::Architecture<ISA>::virtual_address_t;
  using minimal_addressable_unit_t =
      typename isa::Architecture<ISA>::minimal_addressable_unit_t;

public:
  struct MemoryRegion {
    virtual_address_t start;
    virtual_address_t end;
    Permission permissions;
  };

private:
  MemoryRegion text_segment;
  MemoryRegion data_segment;
  MemoryRegion heap;
  MemoryRegion stack;
  virtual_address_t heap_break;             // Current heap allocation point
  std::vector<MemoryRegion> mapped_regions; // Additional mapped regions
};

template <isa::instruction_set ISA> class Context {
  enum class PrivilegeLevel : uint8_t { kUser = 0, kSupervisor, kMachine };
  using virtual_address_t = typename isa::Architecture<ISA>::virtual_address_t;
  using instruction_size_t =
      typename isa::Architecture<ISA>::instruction_size_t;
  using minimal_addressable_unit_t =
      typename isa::Architecture<ISA>::minimal_addressable_unit_t;

public:
  Context()
      : program_counter(0), stack_pointer(0), instruction_register(0),
        general_purpose_registers{}, status_flags{} {}

private:
  virtual_address_t program_counter;
  virtual_address_t stack_pointer;
  instruction_size_t instruction_register;
  std::pair<virtual_address_t, virtual_address_t> memory_bounds;
  std::array<virtual_address_t,
             isa::Architecture<ISA>::general_purpose_register_count>
      general_purpose_registers;
  PrivilegeLevel privilege_level = PrivilegeLevel::kUser;

private:
  union {
    struct {
      uint8_t carry : 1;
      uint8_t zero : 1;
      uint8_t negative : 1;
      uint8_t overflow : 1;
      uint8_t interrupt_enable : 1;
      uint8_t supervisor : 1;
      uint8_t reserved : 2;
    } bits;
    minimal_addressable_unit_t raw;
  } status_flags;

  // Debug and profiling
  struct DebugInfo {
    virtual_address_t last_instruction_address;
    uint64_t instruction_count;
    uint64_t cycle_count;
  } debug_info;
};

template <isa::instruction_set ISA> class Task {
  using self_type = Task<ISA>;
  using clock_type = std::chrono::steady_clock;
  using time_point = clock_type::time_point;

public:
  enum class State : uint8_t {
    kNew = 0,
    kReady,
    kRunning,
    kWaiting,
    kTerminated,
  };

  using pid_t = uint32_t;

  Task()
      : pid_(auxilia::id::get()), state_(State::kNew), context_{},
        creation_time_(clock_type::now()) {
    spdlog::info("Task created with PID: {}, start time: {}",
                 pid_,
                 std::format("{}", std::chrono::system_clock::now()));
  }

public:
  pid_t id() const noexcept { return pid_; }
  State state() const noexcept { return state_; }
  auto context(this auto &&self) noexcept -> decltype(auto) {
    return self.context_;
  }

private:
  // Core task data
  pid_t pid_;
  State state_;
  Context<ISA> context_;
  AddressSpace<ISA> address_space_;

  // Task hierarchy
  std::shared_ptr<self_type> parent_;
  std::vector<std::shared_ptr<self_type>> children_;

  // TODO: implement the following, currently not used

  // Scheduling metadata
  uint8_t priority_{0};
  uint32_t time_slice_{0};
  time_point creation_time_;
  time_point last_run_time_;
  uint64_t total_cpu_time_{0};

  // Resource management
  struct ResourceLimits {
    size_t max_memory;
    size_t max_files;
    uint32_t max_threads;
  } limits_;

  // I/O and resources
  std::vector<int> file_descriptors_;
  std::optional<int32_t> exit_code_;
};
} // namespace accat::luce
