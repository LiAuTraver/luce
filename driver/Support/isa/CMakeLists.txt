project(Support.isa
    LANGUAGES C CXX
    DESCRIPTION "shared items between luce project"
)
add_library(Support.isa SHARED)
file(GLOB_RECURSE LUCE_ISA_SOURCES "*.cpp")
file(GLOB_RECURSE LUCE_ISA_HEADERS "*.hpp")
target_compile_features(Support.isa PUBLIC cxx_std_23)
target_sources(Support.isa PRIVATE
  ${LUCE_ISA_SOURCES}
  ${LUCE_ISA_HEADERS}
)
target_link_libraries(Support.isa PRIVATE
  external_deps
)
target_precompile_headers(Support.isa PRIVATE
  ${LUCE_EXTERNAL_DEPS_PCH}
)
add_llvm_deps_for(Support.isa)

add_folder(Support)
