#ifndef MARLAIS_GC_H
#define MARLAIS_GC_H

#include <marlais/common.h>

extern void marlais_register_gc (void);

extern void marlais_gc_collect (void);
extern void marlais_gc_report (void);

#endif
