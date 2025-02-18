# Luce Emulator
> Luce, means light, luminous in Italian.

A simple emulator for the riscv32 instruction set.

> WIP, so flaws and bugs are expected.

## Build
### Bazel 
#### Linux
```bash
pip install conan
conan install . --output-folder=conan_installed
# seems the rc was optional, but anyway
bazel --bazelrc=conan_installed/conan_bzl.rc build //tools/luce 
# to get the compile_commands.json for clangd:
bazel run @hedron_compile_commands//:refresh_all
```
#### Windows
Update the `_MSVC_VERSION` and `_WINDOWS_10_SDK_VERSION` variable in [the `BUILD` file](bazel/toolchain/BUILD.bazel) to point to the correct path to the MSVC toolchain.
then same as Linux.
### CMake
vcpkg is used to manage the dependencies.
```bash
vcpkg install fmt spdlog argparse scnlib
cmake --preset "MSVC-Debug" # or Clang-Debug, GNU-Debug
cmake --build build/MSVC-Debug --parallel 32 --target luce
```