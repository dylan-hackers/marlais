/* compiler.h -- see COPYRIGHT for use */

#ifndef MARLAIS_COMPILER_H
#define MARLAIS_COMPILER_H

/* attribute macros */
#ifdef __GNUC__
#define MARLAIS_FUN_CONST    __attribute__((const))
#define MARLAIS_FUN_PURE     __attribute__((pure))
#define MARLAIS_FUN_NORETURN __attribute__((noreturn))
#define MARLAIS_FUN_NONNULL  __attribute__((nonnull))
//#define __marlais_counted_by(_field) __attribute__((__counted_by__(_field)))
#else
#define MARLAIS_FUN_CONST
#define MARLAIS_FUN_PURE
#define MARLAIS_FUN_NORETURN
#define MARLAIS_FUN_NONNULL
//#define __marlais_counted_by(_field)
#endif
#define __marlais_counted_by(_field)

#endif /* !MARLAIS_COMPILER_H */
