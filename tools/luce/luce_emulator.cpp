//===------------ luce emulator main entry point ---------*- C++ -*-===//
//
// Part of the Luce project.
// Licensed under the Apache License v2.0.
//
//===---------------------------------------------------------------===//

#include "init-inl.hpp"

int main(int argc, char **argv) {
  auto args = accat::luce::modernize_args(argc, argv);

  // argument modernization done. now invoke the main function.
  return accat::luce::main({args});
}
