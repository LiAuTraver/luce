#include "init-inl.hpp"

int main(int argc, char **argv) {
  accat::luce::init X{&argc, &argv};
  
  // argument modernization done. now invoke the main function.
  return accat::luce::luce_main({X.args});
}
