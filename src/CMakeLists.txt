
# Class precedence algorithm
set(MARLAIS_CLASS_PRECEDENCE "LL" CACHE STRING
  "Class precedence algorithm to use")
set_property(CACHE MARLAIS_CLASS_PRECEDENCE
  PROPERTY STRINGS "CLOS;LL")
if (MARLAIS_CLASS_PRECEDENCE STREQUAL CLOS)
  set(MARLAIS_CLASS_PRECEDENCE_CLOS ON)
endif ()
if (MARLAIS_CLASS_PRECEDENCE STREQUAL LL)
  set(MARLAIS_CLASS_PRECEDENCE_LL ON)
endif ()

# Object model
set(MARLAIS_OBJECT_MODEL "SMALL" CACHE STRING
  "Object model to use")
set_property(CACHE MARLAIS_OBJECT_MODEL
  PROPERTY STRINGS "LARGE;SMALL")
if (MARLAIS_OBJECT_MODEL STREQUAL LARGE)
  set(MARLAIS_OBJECT_MODEL_LARGE ON)
endif ()
if (MARLAIS_OBJECT_MODEL STREQUAL SMALL)
  set(MARLAIS_OBJECT_MODEL_SMALL ON)
endif ()

# Standard character type
set(MARLAIS_STANDARD_CHARACTER "BYTE" CACHE STRING
  "Default character and string type")
set_property(CACHE MARLAIS_STANDARD_CHARACTER
  PROPERTY STRINGS "BYTE;WIDE;UNICODE")
if (MARLAIS_STANDARD_CHARACTER STREQUAL BYTE)
  set(MARLAIS_STANDARD_CHARACTER_BYTE ON)
endif ()
if (MARLAIS_STANDARD_CHARACTER STREQUAL WIDE)
  set(MARLAIS_STANDARD_CHARACTER_WIDE ON)
endif ()
if (MARLAIS_STANDARD_CHARACTER STREQUAL UNICODE)
  set(MARLAIS_STANDARD_CHARACTER_UNICODE ON)
endif ()

# Bignum support
option(MARLAIS_ENABLE_BIGNUM "Enable GMP bignum support" ON)
if (MARLAIS_ENABLE_BIGNUM)
  set (MARLAIS_BIGNUM_SOURCES bignum.c)
endif ()

# READLINE support
option(MARLAIS_ENABLE_READLINE "Enable readline command-line editor" ON)

# ICU unicode support
option(MARLAIS_ENABLE_UCHAR "Enable unicode character support (uses ICU)" ON)
if (MARLAIS_ENABLE_UCHAR)
  set (MARLAIS_UNICODE_SOURCES unicode.c)
endif ()

# Wide character support
option(MARLAIS_ENABLE_WCHAR "Enable wide character support" ON)

configure_file(config.h.in ../include/marlais/config.h)

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
  float.c
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

  ${MARLAIS_BIGNUM_SOURCES}
  ${MARLAIS_UNICODE_SOURCES}
)

# actual name
set_target_properties(marlais_lib
  PROPERTIES OUTPUT_NAME marlais)

# generated includes
target_include_directories(marlais_lib PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# includes
target_include_directories(marlais_lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../include)
target_include_directories(marlais_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
file(GLOB_RECURSE MARLAIS_INCLUDES
  "${CMAKE_CURRENT_BINARY_DIR}/../include/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h")
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
