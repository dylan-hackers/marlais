cmake_minimum_required(VERSION 3.0.0)

project(marlais)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(BDWGC REQUIRED bdw-gc)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
    OR CMAKE_CXX_COMPILER_ID STREQUAL "CLang")
  add_compile_options(-Wall -Wextra -Wno-unused)
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  option(GCC_ENABLE_BOUNDSCHECK "Enable bounds checking" ON)
  if (GCC_ENABLE_BOUNDSCHECK)
    add_compile_options(-fbounds-check)
  endif ()
endif ()

add_subdirectory(src)
add_subdirectory(bin)

install(
  TARGETS marlais_lib marlais_bin
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include/marlais
)
