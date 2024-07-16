
# Class precedence algorithm
set(MARLAIS_CONFIG_CLASSPREC "LL" CACHE STRING
  "Class precedence algorithm to use")
set_property(CACHE MARLAIS_CONFIG_CLASSPREC
  PROPERTY STRINGS "CLOS;LL")
if (MARLAIS_CONFIG_CLASSPREC STREQUAL CLOS)
  set(MARLAIS_CLASS_PRECEDENCE_CLOS ON)
endif ()
if (MARLAIS_CONFIG_CLASSPREC STREQUAL LL)
  set(MARLAIS_CLASS_PRECEDENCE_LL ON)
endif ()

# Object model
set(MARLAIS_CONFIG_OBJMODEL "SMALL" CACHE STRING
  "Object model to use")
set_property(CACHE MARLAIS_CONFIG_OBJMODEL
  PROPERTY STRINGS "LARGE;SMALL")
if (MARLAIS_CONFIG_OBJMODEL STREQUAL LARGE)
  set(MARLAIS_OBJECT_MODEL_LARGE ON)
endif ()
if (MARLAIS_CONFIG_OBJMODEL STREQUAL SMALL)
  set(MARLAIS_OBJECT_MODEL_SMALL ON)
endif ()

# Standard character type
set(MARLAIS_CONFIG_STDCHAR "BYTE" CACHE STRING
  "Default character and string type")
set_property(CACHE MARLAIS_CONFIG_STDCHAR
  PROPERTY STRINGS "BYTE;WIDE;UNICODE")
if (MARLAIS_CONFIG_STDCHAR STREQUAL BYTE)
  set(MARLAIS_STANDARD_CHARACTER_BYTE ON)
endif ()
if (MARLAIS_CONFIG_STDCHAR STREQUAL WIDE)
  set(MARLAIS_STANDARD_CHARACTER_WIDE ON)
endif ()
if (MARLAIS_CONFIG_STDCHAR STREQUAL UNICODE)
  set(MARLAIS_STANDARD_CHARACTER_UNICODE ON)
endif ()

# EFLOAT support
option(MARLAIS_ENABLE_EFLOAT "Enable extended float precision" ON)

# WCHAR support
option(MARLAIS_ENABLE_WCHAR "Enable wide character support" ON)

# GMP bignum support
option(MARLAIS_ENABLE_GMP "Enable GMP bignum support" ON)
if (MARLAIS_ENABLE_GMP)
  set (MARLAIS_GMP_SOURCES prims/bignum.c)
endif ()

# READLINE support
option(MARLAIS_ENABLE_READLINE "Enable readline command-line editor" ON)

# ICU unicode support
option(MARLAIS_ENABLE_ICU "Enable ICU unicode support" ON)
if (MARLAIS_ENABLE_ICU)
  set (MARLAIS_ICU_SOURCES prims/unicode.c)
endif ()

# Generate config header
configure_file(config.h.in ../include/marlais/config.h)

# Generate parser
bison_target(
  PARSER parser/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.gen.c
  COMPILE_FLAGS "-v -t -p marlais_yy"
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.gen.h
  REPORT_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.gen.log
)
# Generate lexer
flex_target(
  LEXER parser/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.gen.c
  COMPILE_FLAGS "-v -I -P marlais_yy"
  DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/lexer.gen.h
)
# Link parser and lexer
add_flex_bison_dependency(LEXER PARSER)

# Main library
add_library(marlais_lib
  core/alloc.c
  core/apply.c
  core/classprec.c
  core/context.c
  core/debug.c
  core/env.c
  core/eval.c
  core/file.c
  core/gc.c
  core/globals.c
  core/object.c
  core/syntax.c
  object/array.c
  object/bytevector.c
  object/condition.c
  object/deque.c
  object/foreignptr.c
  object/list.c
  object/module.c
  object/prim.c
  object/stdio.c
  object/sequence.c
  object/stdio.c
  object/symbol.c
  object/type.c
  object/values.c
  object/vector.c
  parser/lexer.c
  parser/parser.c
  prims/array.c
  prims/boolean.c
  prims/bytevector.c
  prims/character.c
  prims/class.c
  prims/condition.c
  prims/deque.c
  prims/float.c
  prims/foreign.c
  prims/function.c
  prims/gc.c
  prims/integer.c
  prims/list.c
  prims/module.c
  prims/print.c
  prims/slot.c
  prims/stdio.c
  prims/stream.c
  prims/string.c
  prims/symbol.c
  prims/sys.c
  prims/table.c
  prims/type.c
  prims/values.c
  prims/vector.c

  ${BISON_PARSER_OUTPUTS}
  ${FLEX_LEXER_OUTPUTS}

  ${MARLAIS_GMP_SOURCES}
  ${MARLAIS_ICU_SOURCES}
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
