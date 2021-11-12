#ifndef MARLAIS_GMP_H
#define MARLAIS_GMP_H

#include <marlais/common.h>

extern void marlais_initialize_gmp(void);
extern void marlais_register_gmp(void);

extern Object marlais_make_mpf_from_number(Object value);
extern Object marlais_make_mpf_from_string(const char *str, int base);

extern Object marlais_make_mpq_from_number(Object value);
extern Object marlais_make_mpq_from_string(const char *str, int base);

extern Object marlais_make_mpz_from_number(Object value);
extern Object marlais_make_mpz_from_string(const char *str, int base);

#endif
