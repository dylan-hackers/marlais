/* compiler.h -- see COPYRIGHT for use */

#ifndef MARLAIS_COMPILER_H
#define MARLAIS_COMPILER_H

/* attribute macros */
#ifdef __GNUC__
#define MARLAIS_FUN_CONST    __attribute__((const))
#define MARLAIS_FUN_PURE     __attribute__((pure))
#define MARLAIS_FUN_NORETURN __attribute__((noreturn))
#define MARLAIS_FUN_NONNULL  __attribute__((nonnull))
#else
#define MARLAIS_FUN_CONST
#define MARLAIS_FUN_PURE
#define MARLAIS_FUN_NORETURN
#define MARLAIS_FUN_NONNULL
#endif

#endif
