#ifndef MARLAIS_GMP_H
#define MARLAIS_GMP_H

#include <marlais/common.h>

/* Initialize gmp for use with marlais */
extern void marlais_initialize_gmp(void);
/* Register gmp primitives */
extern void marlais_register_gmp(void);

/* Make an <big-float> */
extern Object marlais_make_mpf_from_number(Object value);
/* Make an <big-float> from a byte string */
extern Object marlais_make_mpf_from_string(const char *str, int base);

/* Make an <big-ratio> */
extern Object marlais_make_mpq_from_number(Object value);
/* Make an <big-ratio> from a byte string */
extern Object marlais_make_mpq_from_string(const char *str, int base);

/* Make an <big-integer> */
extern Object marlais_make_mpz_from_number(Object value);
/* Make an <big-integer> from a byte string */
extern Object marlais_make_mpz_from_string(const char *str, int base);

#endif /* !MARLAIS_GMP_H */
