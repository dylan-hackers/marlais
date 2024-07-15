#ifndef MARLAIS_GMP_H
#define MARLAIS_GMP_H

#include <marlais/common.h>

#ifdef MARLAIS_ENABLE_GMP

#include <gmp.h>

struct marlais_bigfloat {
  ObjectHeader header;
  mpf_t mpf;
};

#define MPFVAL(obj) (((struct marlais_bigfloat *)obj)->mpf)

struct marlais_bigratio {
  ObjectHeader header;
  mpq_t mpq;
};

#define MPQVAL(obj) (((struct marlais_bigratio *)obj)->mpq)

struct marlais_biginteger {
  ObjectHeader header;
  mpz_t mpz;
};

#define MPZVAL(obj) (((struct marlais_biginteger *)obj)->mpz)

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

#endif /* MARLAIS_ENABLE_GMP */

#endif /* !MARLAIS_GMP_H */
