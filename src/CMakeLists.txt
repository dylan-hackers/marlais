

# Interpreter configuration
set(MARLAIS_CONFIG_CLASSPREC "LL" CACHE STRING
  "Class precedence algorithm to use")
set_property(CACHE MARLAIS_CONFIG_CLASSPREC
  PROPERTY STRINGS "CLOS;LL")

set(MARLAIS_CONFIG_OBJMODEL "SMALL" CACHE STRING
  "Object model to use")
set_property(CACHE MARLAIS_CONFIG_OBJMODEL
  PROPERTY STRINGS "LARGE;SMALL")

set(MARLAIS_CONFIG_STDSTRING "BYTE" CACHE STRING
  "Default string and character type")
set_property(CACHE MARLAIS_CONFIG_STDSTRING
  PROPERTY STRINGS "BYTE;WIDE;UNICODE")

# GMP support
option(MARLAIS_ENABLE_GMP "Enable GMP multi-precision numbers" ON)
if (MARLAIS_ENABLE_GMP)
  set (MARLAIS_GMP_SOURCES bignum.c)
endif ()

option(MARLAIS_ENABLE_ICU "Enable ICU unicode string support" ON)
if (MARLAIS_ENABLE_ICU)
  set (MARLAIS_ICU_SOURCES unicode.c)
endif ()

# READLINE support
option(MARLAIS_ENABLE_READLINE "Enable readline command-line editor" ON)

# ICU unicode support
option(MARLAIS_ENABLE_UCHAR "Enable unicode character support (uses ICU)" ON)

# Wide character support
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
  bytevector.c
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
  gc.c
  globals.c
  lexer.c
  list.c
  misc.c
  number.c
  object.c
  parser.c
  prim.c
  print.c
  sequence.c
  slot.c
  stdio.c
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

  ${MARLAIS_GMP_SOURCES}
  ${MARLAIS_ICU_SOURCES}
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
# libgc
target_link_libraries(marlais_lib PUBLIC ${BDWGC_LINK_LIBRARIES})
target_include_directories(marlais_lib PRIVATE ${BDWGC_INCLUDE_DIRS})

# libreadline
if (MARLAIS_ENABLE_READLINE)
  target_link_libraries(marlais_lib PUBLIC readline)
endif (MARLAIS_ENABLE_READLINE)
# libgmp
if (MARLAIS_ENABLE_GMP)
  target_link_libraries(marlais_lib PUBLIC gmp)
endif (MARLAIS_ENABLE_GMP)
# libicu
if (MARLAIS_ENABLE_ICU)
  target_link_libraries (marlais_lib PUBLIC ${ICU_LINK_LIBRARIES})
  target_include_directories (marlais_lib PUBLIC ${ICU_INCLUDE_DIRS})
endif (MARLAIS_ENABLE_ICU)
