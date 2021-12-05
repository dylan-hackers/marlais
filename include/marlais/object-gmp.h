
#include <gmp.h>

struct big_float {
  ObjectHeader header;
  mpf_t mpf;
};

#define MPFVAL(obj) (((struct big_float *)obj)->mpf)

struct big_ratio {
  ObjectHeader header;
  mpq_t mpq;
};

#define MPQVAL(obj) (((struct big_ratio *)obj)->mpq)

struct big_integer {
  ObjectHeader header;
  mpz_t mpz;
};

#define MPZVAL(obj) (((struct big_integer *)obj)->mpz)
