cmake_minimum_required(VERSION 3.0.0)

project(marlais)

# Find pkg-config
find_package(PkgConfig REQUIRED)
# Find lexer and parser generators
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# Check for the BDWgarbage collector
pkg_check_modules(BDWGC REQUIRED bdw-gc)
# Check for the ICU unicode library
pkg_check_modules(ICU REQUIRED icu-uc)

# Configuration for CLang and GNU compilers
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
    OR CMAKE_CXX_COMPILER_ID STREQUAL "CLang")

  # Always enable lots of warnings
  add_compile_options(-Wall -Wextra -Wno-unused)

  # Optional link-time optimization
  option(CC_ENABLE_LTO "Enable link-time optimization" OFF)
  if (CC_ENABLE_LTO)
    add_compile_options(-flto)
  endif ()

endif ()

# Configuration for GNU compilers
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

  # Optionally enable bounds checking
  option(CC_ENABLE_BOUNDSCHECK "Enable bounds checking" OFF)
  if (CC_ENABLE_BOUNDSCHECK)
    add_compile_options(-fbounds-check)
  endif ()

endif ()

# Include subdirectories
add_subdirectory(src)
add_subdirectory(bin)

# Define installation
install(
  TARGETS marlais_lib marlais_bin
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include/marlais
)
