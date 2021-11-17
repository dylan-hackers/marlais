#ifndef MARLAIS_GMP_H
#define MARLAIS_GMP_H

#include <marlais/common.h>

/* Initialize gmp for use with marlais */
extern void marlais_initialize_gmp(void);
/* Register gmp primitives */
extern void marlais_register_gmp(void);

/* Make an <mp-float> */
extern Object marlais_make_mpf_from_number(Object value);
/* Make an <mp-float> from a byte string */
extern Object marlais_make_mpf_from_string(const char *str, int base);

/* Make an <mp-ratio> */
extern Object marlais_make_mpq_from_number(Object value);
/* Make an <mp-ratio> from a byte string */
extern Object marlais_make_mpq_from_string(const char *str, int base);

/* Make an <mp-integer> */
extern Object marlais_make_mpz_from_number(Object value);
/* Make an <mp-integer> from a byte string */
extern Object marlais_make_mpz_from_string(const char *str, int base);

#endif /* !MARLAIS_GMP_H */
