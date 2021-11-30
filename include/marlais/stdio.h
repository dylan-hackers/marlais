#ifndef MARLAIS_STDIO_H
#define MARLAIS_STDIO_H

#include <marlais/common.h>

extern void marlais_register_stdio (void);

extern Object marlais_make_stdio_handle (FILE *fp, bool owned);

#endif /* !MARLAIS_STDIO_H */
