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
  auto ptr = new Context();
  ptr->memory_bounds = {address_space_.static_regions.text_segment.start,
                        address_space_.dynamic_regions.heap_break};
  ptr->program_counter = address_space_.static_regions.text_segment.start;
  ptr->stack_pointer = address_space_.dynamic_regions.stack.end;
  ptr->privilege_level = Context::PrivilegeLevel::kUser;
  context_ = std::make_shared<Context>(*ptr);
  return *this;
}
} // namespace accat::luce
