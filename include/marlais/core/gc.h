#ifndef MARLAIS_GC_H
#define MARLAIS_GC_H

#include <marlais/common.h>

/* Initialize the garbage collector */
extern void marlais_initialize_gc (void);
/* Register garbage collector primitives */
extern void marlais_register_gc (void);

/* Perform a full garbage collection */
extern void marlais_gc_collect (void);
/* Report on garbage collector state */
extern void marlais_gc_report (void);

#endif /* !MARLAIS_GC_H */
