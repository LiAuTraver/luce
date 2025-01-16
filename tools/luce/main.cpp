//===- main.cpp - luce emulator main entry point ---------*- C++ -*-===//
//
// Part of the Luce project.
// Licensed under the Apache License v2.0.
//
//===---------------------------------------------------------------===//

#include "init-inl.hpp"

int main(const int argc,
         const char *const *const argv,
         [[maybe_unused]] const char *const *const envp) {
  accat::luce::init _{&argc, &argv};

  // argument modernization done. now invoke the main function.
  return accat::luce::luce_main({_.args});
}
