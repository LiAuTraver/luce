//===- main.cpp - luce emulator main entry point ---------*- C++ -*-===//
//
// Part of the Luce project.
// Licensed under the Apache License v2.0.
//
//===---------------------------------------------------------------===//

#include "init-inl.hpp"

int main(int argc, char **argv, [[maybe_unused]] char **envp) {
  accat::luce::init X{&argc, &argv};

  // argument modernization done. now invoke the main function.
  return accat::luce::luce_main({X.args});
}
