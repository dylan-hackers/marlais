#ifndef MARLAIS_DEBUG_H
#define MARLAIS_DEBUG_H

#include <marlais/common.h>

extern void marlais_initialize_debug(void);

extern void marlais_register_debug(void);

extern Object marlais_debugger(void);

extern jmp_buf *marlais_push_error (void);
extern jmp_buf *marlais_pop_error (void);

#endif /* !MARLAIS_DEBUG_H */
