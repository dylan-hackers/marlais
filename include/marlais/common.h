/* common.h -- see COPYRIGHT for use */

#ifndef MARLAIS_COMMON_H
#define MARLAIS_COMMON_H

#include <marlais/config.h>

#include <setjmp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef MARLAIS_ENABLE_WCHAR
#include <wchar.h>
#endif

#include <marlais/compiler.h>
#include <marlais/object.h>
#include <marlais/globals.h>
#include <marlais/error.h>
#include <marlais/boolean.h>
#include <marlais/type.h>

#endif
