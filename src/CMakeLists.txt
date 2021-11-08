
bison_target(
  PARSER parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.gen.c
  COMPILE_FLAGS "-v -t"
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.gen.h
  REPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.gen.log
)
flex_target(
  LEXER lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.gen.c
  COMPILE_FLAGS "-v -I"
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/lexer.gen.h
)
add_flex_bison_dependency(LEXER PARSER)

add_executable(marlais
  alloc.c
  apply.c
  array.c
  boolean.c
  bytestring.c
  character.c
  class.c
  classprec.c
  deque.c
  env.c
  error.c
  eval.c
  file.c
  foreign.c
  foreign_ptr.c
  function.c
  globals.c
  keyword.c
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
  symbol.c
  syntax.c
  sys.c
  table.c
  values.c
  vector.c

  ${BISON_PARSER_OUTPUTS}
  ${FLEX_LEXER_OUTPUTS}

  main.c
)

target_include_directories(marlais PUBLIC ../include)

# libdl
target_link_libraries(marlais PUBLIC dl)

# libm
target_link_libraries(marlais PUBLIC m)

# libreadline
target_link_libraries(marlais PUBLIC readline)

# libgc
target_link_libraries(marlais PUBLIC ${BDWGC_LINK_LIBRARIES})
target_include_directories(marlais PRIVATE ${BDWGC_INCLUDE_DIRS})
