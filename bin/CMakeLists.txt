
add_executable(marlais_bin marlais.c)

set_target_properties(
  marlais_bin PROPERTIES
  OUTPUT_NAME marlais)

target_include_directories(
  marlais_lib PUBLIC ../include)

target_link_libraries(
  marlais_bin PUBLIC marlais_lib)
