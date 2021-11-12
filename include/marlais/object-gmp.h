
#include <gmp.h>

struct mp_float {
  ObjectHeader header;
  mpf_t mpf;
};

#define MPFVAL(obj) (((struct mp_float *)obj)->mpf)

struct mp_ratio {
  ObjectHeader header;
  mpq_t mpq;
};

#define MPQVAL(obj) (((struct mp_ratio *)obj)->mpq)

struct mp_integer {
  ObjectHeader header;
  mpz_t mpz;
};

#define MPZVAL(obj) (((struct mp_integer *)obj)->mpz)
