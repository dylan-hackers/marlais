cmake_minimum_required(VERSION 3.0.0)

project(marlais)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(BDWGC REQUIRED bdw-gc)

add_subdirectory(src)
add_subdirectory(bin)

install(
  TARGETS marlais_lib marlais_bin
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include/marlais
)
