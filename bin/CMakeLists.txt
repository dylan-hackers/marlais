
add_executable(marlais_bin marlais.c)

target_link_libraries(marlais_bin PUBLIC marlais)

set_target_properties(marlais_bin
  PROPERTIES OUTPUT_NAME marlais)
