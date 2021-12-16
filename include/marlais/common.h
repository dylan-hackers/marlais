/* common.h -- see COPYRIGHT for use */

#ifndef MARLAIS_COMMON_H
#define MARLAIS_COMMON_H

#include <marlais/config.h>
#include <marlais/limits.h>

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

#include <marlais/compiler.h>
#include <marlais/object.h>
#include <marlais/error.h>
#include <marlais/boolean.h>
#include <marlais/float.h>
#include <marlais/list.h>
#include <marlais/module.h>
#include <marlais/number.h>
#include <marlais/symbol.h>
#include <marlais/type.h>

#endif /* MARLAIS_COMMON_H */
