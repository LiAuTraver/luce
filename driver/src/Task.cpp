#include "deps.hh"

#include "luce/Task.hpp"

namespace accat::luce {
Task::Task(Mediator *parent)
    : Component(parent), state_(State::kNew),
      context_{}, address_space_(), creation_time_(clock_type::now()),
      limits_(), state(this), address_space(this) {
}
Task &Task::set_address_space(const AddressSpace &newAddressSpace) noexcept {
  address_space_ = newAddressSpace;
  context_ = Context();
  context_.memory_bounds = {address_space_.static_regions.text_segment.start,
                        address_space_.dynamic_regions.heap_break};
  context_.program_counter.num() = address_space_.static_regions.text_segment.start;
  context_.stack_pointer.num() = address_space_.dynamic_regions.stack.end;
  context_.privilege_level = Context::PrivilegeLevel::kUser;
  return *this;
}
} // namespace accat::luce
