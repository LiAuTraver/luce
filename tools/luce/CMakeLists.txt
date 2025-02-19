project(luce
  LANGUAGES CXX
  DESCRIPTION "An riscv64 emulator"
)
include(folders)
include(dependencies)
include(llvm)

add_executable(luce)
target_sources(luce PUBLIC
  alter_args-inl.h
  init-inl.hpp
  luce_emulator.cpp
)
target_link_libraries(luce PUBLIC

  # do not directly use `driver` otherwise it'll result multiple defined symbols when linking
  $<TARGET_OBJECTS:driver>
  # donno why but needed even though driver already includes them
  external_deps
)
target_compile_features(luce PUBLIC cxx_std_23)
add_folder(Tools)