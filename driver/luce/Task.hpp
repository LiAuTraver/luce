#pragma once

#include "utils/Pattern.hpp"

#include <spdlog/spdlog.h>
#include <accat/auxilia/auxilia.hpp>
#include <accat/auxilia/details/Property.hpp>
#include <accat/auxilia/details/Status.hpp>

#include "isa/architecture.hpp"

namespace accat::luce {
enum class [[clang::flag_enum]] Permission : uint8_t {
  kNone = 0,
  kRead = 1,
  kWrite = 2,
  kExecute = 4,
  kReadWrite = kRead | kWrite,
  kReadExecute = kRead | kExecute,
  kReadWriteExecute = kRead | kWrite | kExecute,
};
AC_BITMASK_OPS(Permission);

class Context {
  static_assert(sizeof(std::byte) == sizeof(isa::minimal_addressable_unit_t),
                "current implementation requires std::byte to be same size as "
                "minimal_addressable_unit_t");
  using vaddr_t = isa::virtual_address_t;
  using register_t = std::array<std::byte,
                                sizeof(isa::virtual_address_t) /
                                    sizeof(isa::minimal_addressable_unit_t)>;

public:
  enum class PrivilegeLevel : uint8_t { kUser = 0, kSupervisor, kMachine };

public:
  void restart() {
    program_counter = 0x0;
    stack_pointer = 0x0;
    instruction_register = {};
    memory_bounds = {};
    general_purpose_registers.fill({});
    privilege_level = PrivilegeLevel::kUser;
  }

public:
  vaddr_t program_counter = 0x0;
  vaddr_t stack_pointer = 0x0;
  register_t instruction_register = {};
  std::pair<vaddr_t, vaddr_t> memory_bounds = {};
  std::array<register_t, isa::general_purpose_register_count>
      general_purpose_registers = {};
  PrivilegeLevel privilege_level = PrivilegeLevel::kUser;

private:
  // union {
  //   struct alignas(1) {
  //     uint8_t carry : 1;
  //     uint8_t zero : 1;
  //     uint8_t negative : 1;
  //     uint8_t overflow : 1;
  //     uint8_t interrupt_enable : 1;
  //     uint8_t supervisor : 1;
  //     uint8_t reserved : 2;
  //   } bits;
  //   isa::minimal_addressable_unit_t raw;
  // } status_flags;
  //
  // // Debug and profiling
  // struct DebugInfo {
  //   vaddr_t last_instruction_address;
  //   uint64_t instruction_count;
  //   uint64_t cycle_count;
  // } debug_info;
};
struct AddressSpace {
  friend class Task;
  using vaddr_t = isa::virtual_address_t;
  struct MemoryRegion {
    vaddr_t start;
    vaddr_t end;
    Permission permissions;
  };
  struct StaticRegion {
    MemoryRegion text_segment;
    MemoryRegion data_segment;
  };
  struct DynamicRegion {
    MemoryRegion stack;
    vaddr_t heap_break; // Current heap allocation point
    MemoryRegion heap;
  };
  StaticRegion static_regions;
  DynamicRegion dynamic_regions;
  std::vector<MemoryRegion> mapped_regions; // Additional mapped regions
};

class Task : public Component {
  using self_type = Task;
  using clock_type = std::chrono::steady_clock;
  using time_point = clock_type::time_point;

public:
  enum class State : uint8_t {
    kNew = 0,
    kReady,
    kRunning,
    kWaiting,
    kPaused,
    kTerminated,
  };

  Task(Mediator * = nullptr);

public:
  // auxilia::Status run() {
  //   precondition(mediator, "Task has no mediator. Check your code.")
  //   if (state_ == State::kRunning) {
  //     spdlog::warn("Task already running");
  //     return {};
  //   }
  //   if (state_ != State::kNew && state_ != State::kReady) {
  //     spdlog::warn("Task not in a runnable state");
  //     return {};
  //   }
  //   state_ = State::kRunning;
  //   return send(Event::kRunTask);
  // }
  void pause() {}
  void resume() {}
  /// @brief (forcefully) terminate the task
  void terminate() {}
  void restart() {
    context_->restart();
    context_->program_counter = address_space_.static_regions.text_segment.start;
    state_ = State::kNew;
    time_slice_ = 0;
    total_cpu_time_ = 0;
    creation_time_ = clock_type::now();
    last_run_time_ = creation_time_;
  }

private:
  // Core task data
  State state_;
  std::shared_ptr<Context> context_;
  AddressSpace address_space_;

  // Task hierarchy
  std::shared_ptr<self_type> parent_;
  std::vector<std::shared_ptr<self_type>> children_;

  // TODO: implement the following, currently not used

  // Scheduling metadata
  uint8_t priority_{0};
  uint32_t time_slice_{0};
  time_point creation_time_;
  time_point last_run_time_ = creation_time_;
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

  /// properties
private:
  State get_state() const noexcept {
    return state_;
  }
  Task &set_state(const State &newState) noexcept {
    state_ = newState;
    return *this;
  }

  const AddressSpace &get_address_space() const noexcept {
    return address_space_;
  }
  Task &set_address_space(const AddressSpace &) noexcept;

public:
  auto context(this auto &&self) noexcept -> decltype(auto) {
    return self.context_;
  }

public:
  auxilia::Property<Task, State, Task &, &Task::get_state, &Task::set_state>
      state;
  auxilia::Property<Task,
                    const AddressSpace &,
                    Task &,
                    &Task::get_address_space,
                    &Task::set_address_space>
      address_space;

private:
  friend struct accat::auxilia::
      Property<Task, State, Task &, &Task::get_state, &Task::set_state>;
  friend struct accat::auxilia::Property<Task,
                                         const AddressSpace &,
                                         Task &,
                                         &Task::get_address_space,
                                         &Task::set_address_space>;
};
} // namespace accat::luce
