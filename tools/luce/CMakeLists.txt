project(luce
    LANGUAGES CXX
    DESCRIPTION "An riscv64 emulator"
)
include(${LUCE_PROJECT_ROOT_DIR}/cmake/folders.cmake)
include(${LUCE_PROJECT_ROOT_DIR}/cmake/dependencies.cmake)
include(${LUCE_PROJECT_ROOT_DIR}/cmake/llvm.cmake)

add_executable(luce)
target_sources(luce PUBLIC
  alter_args-inl.h
  init-inl.hpp
  main.cpp
)
target_link_libraries(luce PUBLIC

  # do not directly use `driver` otherwise it'll result multiple defined symbols when linking
  $<TARGET_OBJECTS:driver>

  # donno why but needed even though driver already includes them
  external_deps
)
target_compile_features(luce PUBLIC cxx_std_23)
add_llvm_deps_for(luce)
add_folder(Tools)