/* common.h -- see COPYRIGHT for use */

#ifndef MARLAIS_COMMON_H
#define MARLAIS_COMMON_H

#include <marlais/config.h>
#include <marlais/core/limits.h>

#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <inttypes.h>

#ifdef MARLAIS_ENABLE_WCHAR
#include <wchar.h>
#endif

#ifdef MARLAIS_ENABLE_UCHAR
#include <unicode/uchar.h>
#endif

#include <marlais/core/compiler.h>
#include <marlais/core/object.h>
#include <marlais/core/alloc.h>
#include <marlais/core/compare.h>
#include <marlais/core/error.h>
#include <marlais/core/print.h>
#include <marlais/object/character.h>
#include <marlais/object/float.h>
#include <marlais/object/integer.h>
#include <marlais/object/module.h>
#include <marlais/object/symbol.h>
#include <marlais/object/type.h>
#include <marlais/object/values.h>
#include <marlais/object/string.h>
#include <marlais/object/list.h>

#endif /* MARLAIS_COMMON_H */
