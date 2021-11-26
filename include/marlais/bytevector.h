#ifndef MARLAIS_BYTEVECTOR_H
#define MARLAIS_BYTEVECTOR_H

#include <marlais/common.h>

extern void marlais_register_bytevector (void);

/* Make a <byte-vector> */
extern Object marlais_make_bytevector (int size, uint8_t fill);
/* Entrypoint for make(<byte-vector>) */
extern Object marlais_make_bytevector_entrypoint (Object args);

#endif /* !MARLAIS_BYTEVECTOR_H */
