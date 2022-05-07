#ifndef MARLAIS_STDIO_H
#define MARLAIS_STDIO_H

#include <marlais/common.h>

struct marlais_stdio_handle {
  ObjectHeader header;
  bool owned;
  FILE *file;
};

#define STDIOOWNEDP(obj) (((struct marlais_stdio_handle *)obj)->owned)
#define STDIOFILE(obj) (((struct marlais_stdio_handle *)obj)->file)

extern void marlais_register_stdio (void);

extern Object marlais_make_stdio_handle (FILE *fp, bool owned);

#endif /* !MARLAIS_STDIO_H */
