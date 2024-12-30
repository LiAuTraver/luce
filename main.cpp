
#include "init-inl.hpp"

namespace accat::luce {
[[nodiscard]] int luce_main([[maybe_unused]] std::span<const std::string_view>);
}

int main(int argc, char **argv) {
  accat::luce::init X{&argc, &argv};

  // argumet modernization done. now invoke the main function.
  return accat::luce::luce_main({X.args});
}

