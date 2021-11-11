
option(MARLAIS_ENABLE_WCHAR "Enable wide character support" ON)

# Generate parser
bison_target(
  PARSER parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.gen.c
  COMPILE_FLAGS "-v -t -p marlais_yy"
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.gen.h
  REPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.gen.log
)
# Generate lexer
flex_target(
  LEXER lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.gen.c
  COMPILE_FLAGS "-v -I -P marlais_yy"
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/lexer.gen.h
)
# Link parser and lexer
add_flex_bison_dependency(LEXER PARSER)

# Main library
add_library(marlais_lib
  alloc.c
  apply.c
  array.c
  boolean.c
  character.c
  class.c
  classprec.c
  context.c
  deque.c
  env.c
  error.c
  eval.c
  file.c
  foreign.c
  foreign_ptr.c
  function.c
  globals.c
  lexer.c
  list.c
  misc.c
  number.c
  object.c
  parser.c
  prim.c
  print.c
  read.c
  sequence.c
  slot.c
  stream.c
  string.c
  symbol.c
  syntax.c
  sys.c
  table.c
  type.c
  values.c
  vector.c

  ${BISON_PARSER_OUTPUTS}
  ${FLEX_LEXER_OUTPUTS}
)

# actual name
set_target_properties(marlais_lib
  PROPERTIES OUTPUT_NAME marlais)

# includes
target_include_directories(marlais_lib PUBLIC ../include)
file(GLOB_RECURSE MARLAIS_INCLUDES "../include/*.h")
set_target_properties(marlais_lib
  PROPERTIES PUBLIC_HEADER "${MARLAIS_INCLUDES}")

# libdl
target_link_libraries(marlais_lib PUBLIC dl)
# libm
target_link_libraries(marlais_lib PUBLIC m)
# libreadline
target_link_libraries(marlais_lib PUBLIC readline)
# libgc
target_link_libraries(marlais_lib PUBLIC ${BDWGC_LINK_LIBRARIES})
target_include_directories(marlais_lib PRIVATE ${BDWGC_INCLUDE_DIRS})
