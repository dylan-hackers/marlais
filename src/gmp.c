
#include <marlais/gmp.h>

#include <marlais/alloc.h>
#include <marlais/env.h>
#include <marlais/number.h>
#include <marlais/prim.h>
#include <marlais/symbol.h>

#include <gmp.h>

/* Internal functions */

/* Allocation wrappers for GMP memory management */
static void *gmp_gc_alloc (size_t size);
static void *gmp_gc_realloc (void *obj, size_t old_size, size_t new_size);
static void  gmp_gc_free (void *obj, size_t old_size);

/* Primitives */

/* Declaration macros (matching definition macros in implementation) */
#define DECLARE_UNARY(_mtype, _name)                                \
  static Object prim_ ## _mtype ## _ ## _name (Object a);           \
  static Object prim_ ## _mtype ## _ ## _name ## _bang (Object res, Object a);
#define DECLARE_BINARY(_mtype, _name)                               \
  static Object prim_ ## _mtype ## _ ## _name (Object a, Object b); \
  static Object prim_ ## _mtype ## _ ## _name ## _bang (Object res, Object a, Object b);
#define DECLARE_PREDICATE(_mtype, _name)                        \
  static Object prim_ ## _mtype ## _ ## _name ## _p (Object a);
#define DECLARE_COMPARE(_mtype)                                     \
  static Object prim_ ## _mtype ## _lessthan (Object a, Object b);  \
  static Object prim_ ## _mtype ## _equal (Object a, Object b);

/* Primitives that operate on <mp-float> */
static Object prim_string_to_mpf(Object str, Object base);
static Object prim_mpf_precision(Object obj);
static Object prim_mpf_precision_setter(Object obj, Object value);
static Object prim_mpf_set_bang(Object obj, Object value);
DECLARE_COMPARE(mpf);
DECLARE_PREDICATE(mpf, zero);
DECLARE_PREDICATE(mpf, positive);
DECLARE_PREDICATE(mpf, negative);
DECLARE_BINARY(mpf, add);
DECLARE_BINARY(mpf, sub);
DECLARE_BINARY(mpf, mul);
DECLARE_BINARY(mpf, div);
DECLARE_BINARY(mpf, pow);
DECLARE_UNARY(mpf, neg);
DECLARE_UNARY(mpf, abs);
DECLARE_UNARY(mpf, ceil);
DECLARE_UNARY(mpf, floor);
DECLARE_UNARY(mpf, trunc);
DECLARE_UNARY(mpf, sqrt);

/* Primitives that operate on <mp-ratio> */
static Object prim_string_to_mpq(Object str, Object base);
static Object prim_mpq_set_bang(Object obj, Object value);
DECLARE_COMPARE(mpq);
DECLARE_PREDICATE(mpq, zero);
DECLARE_PREDICATE(mpq, positive);
DECLARE_PREDICATE(mpq, negative);
DECLARE_BINARY(mpq, add);
DECLARE_BINARY(mpq, sub);
DECLARE_BINARY(mpq, mul);
DECLARE_BINARY(mpq, div);
DECLARE_UNARY(mpq, neg);
DECLARE_UNARY(mpq, abs);
DECLARE_UNARY(mpq, inv);

/* Primitives that operate on <mp-integer> */
static Object prim_string_to_mpz(Object str, Object base);
static Object prim_mpz_set_bang(Object obj, Object value);
DECLARE_COMPARE(mpz);
DECLARE_PREDICATE(mpz, zero);
DECLARE_PREDICATE(mpz, positive);
DECLARE_PREDICATE(mpz, negative);
DECLARE_PREDICATE(mpz, odd);
DECLARE_PREDICATE(mpz, even);
DECLARE_BINARY(mpz, add);
DECLARE_BINARY(mpz, sub);
DECLARE_BINARY(mpz, mul);
DECLARE_BINARY(mpz, cdiv_q);
DECLARE_BINARY(mpz, cdiv_r);
DECLARE_BINARY(mpz, cdiv_qr);
DECLARE_BINARY(mpz, fdiv_q);
DECLARE_BINARY(mpz, fdiv_r);
DECLARE_BINARY(mpz, fdiv_qr);
DECLARE_BINARY(mpz, tdiv_q);
DECLARE_BINARY(mpz, tdiv_r);
DECLARE_BINARY(mpz, tdiv_qr);
DECLARE_UNARY(mpz, neg);
DECLARE_UNARY(mpz, abs);

/* We no longer need the declaration macros */
#undef DECLARE_UNARY
#undef DECLARE_BINARY
#undef DECLARE_PREDICATE
#undef DECLARE_COMPARE

/* Our table of primitives */
static struct primitive gmp_prims[] =
{
  /* <mp-float> */
  {"%number->mpf", prim_1, marlais_make_mpf_from_number},
  {"%string->mpf", prim_2, prim_string_to_mpf},
  {"%mpf-precision", prim_1, prim_mpf_precision},
  {"%mpf-precision-setter", prim_2, prim_mpf_precision_setter},
  {"%mpf<", prim_2, prim_mpf_lessthan},
  {"%mpf=", prim_2, prim_mpf_equal},
  {"%mpf-zero?", prim_1, prim_mpf_zero_p},
  {"%mpf-positive?", prim_1, prim_mpf_positive_p},
  {"%mpf-negative?", prim_1, prim_mpf_negative_p},
  {"%mpf-set!", prim_2, prim_mpf_set_bang},
  {"%mpf-add", prim_2, prim_mpf_add},
  {"%mpf-sub", prim_2, prim_mpf_sub},
  {"%mpf-mul", prim_2, prim_mpf_mul},
  {"%mpf-div", prim_2, prim_mpf_div},
  {"%mpf-pow", prim_2, prim_mpf_pow},
  {"%mpf-neg", prim_1, prim_mpf_neg},
  {"%mpf-abs", prim_1, prim_mpf_abs},
  {"%mpf-ceil", prim_1, prim_mpf_ceil},
  {"%mpf-floor", prim_1, prim_mpf_floor},
  {"%mpf-trunc", prim_1, prim_mpf_trunc},
  {"%mpf-sqrt", prim_1, prim_mpf_sqrt},

  /* <mp-ratio> */
  {"%number->mpq", prim_1, marlais_make_mpq_from_number},
  {"%string->mpq", prim_2, prim_string_to_mpq},
  {"%mpq<", prim_2, prim_mpq_lessthan},
  {"%mpq=", prim_2, prim_mpq_equal},
  {"%mpq-zero?", prim_1, prim_mpq_zero_p},
  {"%mpq-positive?", prim_1, prim_mpq_positive_p},
  {"%mpq-negative?", prim_1, prim_mpq_negative_p},
  {"%mpq-set!", prim_2, prim_mpq_set_bang},
  {"%mpq-add", prim_2, prim_mpq_add},
  {"%mpq-sub", prim_2, prim_mpq_sub},
  {"%mpq-mul", prim_2, prim_mpq_mul},
  {"%mpq-div", prim_2, prim_mpq_div},
  {"%mpq-neg", prim_1, prim_mpq_neg},
  {"%mpq-abs", prim_1, prim_mpq_abs},
  {"%mpq-inv", prim_1, prim_mpq_inv},
  {"%mpq-add!", prim_3, prim_mpq_add_bang},
  {"%mpq-sub!", prim_3, prim_mpq_sub_bang},
  {"%mpq-mul!", prim_3, prim_mpq_mul_bang},
  {"%mpq-div!", prim_3, prim_mpq_div_bang},
  {"%mpq-neg!", prim_2, prim_mpq_neg_bang},
  {"%mpq-abs!", prim_2, prim_mpq_abs_bang},
  {"%mpq-inv!", prim_2, prim_mpq_inv_bang},

  /* <mp-integer> */
  {"%number->mpz", prim_1, marlais_make_mpz_from_number},
  {"%string->mpz", prim_2, prim_string_to_mpz},
  {"%mpz<", prim_2, prim_mpz_lessthan},
  {"%mpz=", prim_2, prim_mpz_equal},
  {"%mpz-zero?", prim_1, prim_mpz_zero_p},
  {"%mpz-positive?", prim_1, prim_mpz_positive_p},
  {"%mpz-negative?", prim_1, prim_mpz_negative_p},
  {"%mpz-odd?", prim_1, prim_mpz_odd_p},
  {"%mpz-even?", prim_1, prim_mpz_even_p},
  {"%mpz-set!", prim_2, prim_mpz_set_bang},
  {"%mpz-add", prim_2, prim_mpz_add},
  {"%mpz-sub", prim_2, prim_mpz_sub},
  {"%mpz-mul", prim_2, prim_mpz_mul},
  {"%mpz-cdiv-q", prim_2, prim_mpz_cdiv_q},
  {"%mpz-cdiv-r", prim_2, prim_mpz_cdiv_r},
  {"%mpz-fdiv-q", prim_2, prim_mpz_fdiv_q},
  {"%mpz-fdiv-r", prim_2, prim_mpz_fdiv_r},
  {"%mpz-tdiv-q", prim_2, prim_mpz_tdiv_q},
  {"%mpz-tdiv-r", prim_2, prim_mpz_tdiv_r},
  {"%mpz-neg", prim_1, prim_mpz_neg},
  {"%mpz-abs", prim_1, prim_mpz_abs},
  {"%mpz-add!", prim_3, prim_mpz_add_bang},
  {"%mpz-sub!", prim_3, prim_mpz_sub_bang},
  {"%mpz-mul!", prim_3, prim_mpz_mul_bang},
  {"%mpz-cdiv-q!", prim_3, prim_mpz_cdiv_q_bang},
  {"%mpz-cdiv-r!", prim_3, prim_mpz_cdiv_r_bang},
  {"%mpz-fdiv-q!", prim_3, prim_mpz_fdiv_q_bang},
  {"%mpz-fdiv-r!", prim_3, prim_mpz_fdiv_r_bang},
  {"%mpz-tdiv-q!", prim_3, prim_mpz_tdiv_q_bang},
  {"%mpz-tdiv-r!", prim_3, prim_mpz_tdiv_r_bang},
  {"%mpz-neg!", prim_2, prim_mpz_neg_bang},
  {"%mpz-abs!", prim_2, prim_mpz_abs_bang},
};

/* Exported functions */

void
marlais_initialize_gmp (void)
{
  /* set memory management functions */
  mp_set_memory_functions(gmp_gc_alloc,
                          gmp_gc_realloc,
                          gmp_gc_free);
}

void
marlais_register_gmp (void)
{
  int num = sizeof (gmp_prims) / sizeof (struct primitive);
  marlais_register_prims (num, gmp_prims);
}

Object
marlais_make_mpf_from_number(Object value)
{
  Object res;

  res = marlais_allocate_object(MPFloat, sizeof(struct mp_float));

#if 0
  /* Simple version using separate init and set */
  mpf_init (MPFVAL(res));
  prim_mpf_set_bang(res, value);
#else
  /* Faster version using combined init and set */
  if (INTEGERP(value)) {
    mpf_init_set_si (MPFVAL(res), INTVAL(value));
  } else if (SFLOATP(value)) {
    mpf_init_set_d (MPFVAL(res), SFLOATVAL(value));
  } else if (DFLOATP(value)) {
    mpf_init_set_d (MPFVAL(res), DFLOATVAL(value));
  } else if (MPFP(value)) {
    mpf_init_set (MPFVAL(res), MPFVAL(value));
  } else {
    mpf_init (MPFVAL(res));
    if(RATIOP(value)) {
      /* create temporary mpq and set from that */
      /* TODO is it faster to set the mpf and divide? */
      mpq_t q;
      mpq_init(q);
      mpq_set_si (q, RATIONUM(value), RATIODEN(value));
      mpf_set_q (MPFVAL(res), MPQVAL(value));
      mpq_clear(q);
    } else if (MPQP(value)) {
      mpf_set_q (MPFVAL(res), MPQVAL(value));
    } else if (MPZP(value)) {
      mpf_set_z (MPFVAL(res), MPZVAL(value));
    } else {
      marlais_fatal("%number->mpf: Wrong first argument");
    }
  }
#endif

  return res;
}

Object
marlais_make_mpf_from_string(const char *str, int base)
{
  Object res;

  res = marlais_allocate_object(MPFloat, sizeof(struct mp_float));

  mpf_init (MPFVAL(res));
  mpf_set_str(MPFVAL(res), str, base);

  return res;
}

Object
marlais_make_mpq_from_number(Object value)
{
  Object res;

  res = marlais_allocate_object(MPRatio, sizeof(struct mp_ratio));

  mpq_init (MPQVAL(res));
  prim_mpq_set_bang(res, value);

  return res;
}

Object
marlais_make_mpq_from_string(const char *str, int base)
{
  Object res;

  res = marlais_allocate_object(MPRatio, sizeof(struct mp_ratio));

  mpq_init (MPQVAL(res));
  mpq_set_str(MPQVAL(res), str, base);

  return res;
}

Object
marlais_make_mpz_from_number(Object value)
{
  Object res;

  res = marlais_allocate_object(MPInteger, sizeof(struct mp_integer));

  mpz_init (MPZVAL(res));
  prim_mpz_set_bang(res, value);

  return res;
}

Object
marlais_make_mpz_from_string(const char *str, int base)
{
  Object res;

  res = marlais_allocate_object(MPInteger, sizeof(struct mp_integer));

  mpz_init (MPZVAL(res));
  mpz_set_str(MPZVAL(res), str, base);

  return res;
}

/* Internal functions */

static void *
gmp_gc_alloc (size_t size) {
  return marlais_allocate_memory(size);
}

static void *
gmp_gc_realloc (void *obj, size_t old_size, size_t new_size) {
  return marlais_reallocate_memory(obj, new_size);
}

static void
gmp_gc_free (void *obj, size_t old_size) {
  marlais_free_memory(obj);
}

/* Primitives */

static Object prim_string_to_mpf(Object str, Object base)
{
  return marlais_make_mpf_from_string(BYTESTRVAL(str), INTVAL(base));
}

static Object
prim_mpf_precision(Object obj)
{
  return marlais_make_integer (mpf_get_prec(MPFVAL(obj)));
}

static Object
prim_mpf_precision_setter(Object obj, Object value)
{
  mpf_set_prec(MPFVAL(obj), INTVAL(value));
  return value;
}

static Object
prim_mpf_set_bang(Object obj, Object value)
{
  if (INTEGERP(value)) {
    mpf_set_si (MPFVAL(obj), INTVAL(value));
  } else if (SFLOATP(value)) {
    mpf_set_d (MPFVAL(obj), SFLOATVAL(value));
  } else if (DFLOATP(value)) {
    mpf_set_d (MPFVAL(obj), DFLOATVAL(value));
  } else if (MPFP(value)) {
    mpf_set (MPFVAL(obj), MPFVAL(value));
  } else if (MPQP(value)) {
    mpf_set_q (MPFVAL(obj), MPQVAL(value));
  } else if (MPZP(value)) {
    mpf_set_z (MPFVAL(obj), MPZVAL(value));
  } else if (RATIOP(value)) {
    /* convert ratio to temporary <mp-ratio> */
    /* TODO do this locally */
    prim_mpf_set_bang(obj, marlais_make_mpq_from_number(value));
  } else {
    marlais_fatal("%mpf-set!: Wrong first argument");
  }
  return value;
}

static Object
prim_string_to_mpq(Object str, Object base)
{
  return marlais_make_mpq_from_string(BYTESTRVAL(str), INTVAL(base));
}

static Object
prim_mpq_set_bang(Object obj, Object value)
{
  if (INTEGERP(value)) {
    mpq_set_si (MPQVAL(obj), INTVAL(value), 1);
  } else if (SFLOATP(value)) {
    mpq_set_d (MPQVAL(obj), SFLOATVAL(value));
  } else if (DFLOATP(value)) {
    mpq_set_d (MPQVAL(obj), DFLOATVAL(value));
  } else if (RATIOP(value)) {
    mpq_set_si (MPQVAL(obj), RATIONUM(value), RATIODEN(value));
  } else if (MPFP(value)) {
    mpq_set_f (MPQVAL(obj), MPFVAL(value));
  } else if (MPQP(value)) {
    mpq_set (MPQVAL(obj), MPQVAL(value));
  } else if (MPZP(value)) {
    mpq_set_z (MPQVAL(obj), MPZVAL(value));
  } else {
    marlais_fatal("%mpq-set!: Wrong first argument");
  }
  return value;
}

static Object
prim_string_to_mpz(Object str, Object base)
{
  return marlais_make_mpz_from_string(BYTESTRVAL(str), INTVAL(base));
}

static Object
prim_mpz_set_bang(Object obj, Object value)
{
  if (INTEGERP(value)) {
    mpz_set_si (MPZVAL(obj), INTVAL(value));
  } else if (SFLOATP(value)) {
    mpz_set_d (MPZVAL(obj), SFLOATVAL(value));
  } else if (DFLOATP(value)) {
    mpz_set_d (MPZVAL(obj), DFLOATVAL(value));
  } else if (MPFP(value)) {
    mpz_set_f (MPZVAL(obj), MPFVAL(value));
  } else if (MPQP(value)) {
    mpz_set_q (MPZVAL(obj), MPQVAL(value));
  } else if (MPZP(value)) {
    mpz_set (MPZVAL(obj), MPZVAL(value));
  } else if (RATIOP(value)) {
    /* convert ratio to temporary <mp-ratio> */
    /* TODO do this locally */
    prim_mpz_set_bang(obj, marlais_make_mpq_from_number(value));
  } else {
    marlais_fatal("%mpz-set!: Wrong first argument");
  }
  return value;
}

typedef struct mp_float   mpf_obj;
typedef struct mp_ratio   mpq_obj;
typedef struct mp_integer mpz_obj;

#define DEFINE_COMPARE_MP_MP(_mt, _dt, _mp, _mg)                        \
  static Object prim_ ## _mt ## _lessthan (Object a, Object b) {        \
    bool res;                                                           \
    if (!_mp(a)) {                                                      \
      marlais_error("%s: Wrong first argument", "%" #_mt "<");          \
    }                                                                   \
    if (!_mp(b)) {                                                      \
      marlais_error("%s: Wrong second argument", "%" #_mt "<");         \
    }                                                                   \
    res = (_mt ## _cmp (_mg(a), _mg(b)) < 0);                           \
    return marlais_make_boolean(res);                                   \
  }                                                                     \
  static Object prim_ ## _mt ## _equal (Object a, Object b) {           \
    bool res;                                                           \
    if (!_mp(a)) {                                                      \
      marlais_error("%s: Wrong first argument", "%" #_mt "=");          \
    }                                                                   \
    if (!_mp(b)) {                                                      \
      marlais_error("%s: Wrong second argument", "%" #_mt "=");         \
    }                                                                   \
    res = (_mt ## _cmp (_mg(a), _mg(b)) == 0);                          \
    return marlais_make_boolean(res);                                   \
  }

#define DEFINE_UNARY_MP(_mt, _dt, _mp, _mg, _op)                        \
  static Object prim_ ## _mt ## _ ## _op ## _bang (Object res, Object a) { \
    /* check */                                                         \
    if (!_mp(a)) {                                                      \
      marlais_error("%s: Wrong argument", "%" #_mt "-" #_op);           \
    }                                                                   \
    /* operate */                                                       \
    _mt ## _ ## _op (_mg(res), _mg(a));                                 \
    /* return */                                                        \
    return res;                                                         \
  }                                                                     \
  static Object prim_ ## _mt ## _ ## _op (Object a) {                   \
    Object res;                                                         \
    /* allocate */                                                      \
    res = marlais_allocate_object(_dt, sizeof(_mt ## _obj));            \
    /* initialize */                                                    \
    _mt ## _init (_mg(res));                                            \
    /* operate and return */                                            \
    return prim_ ## _mt ## _ ## _op ## _bang (res, a);                  \
  }

#define DEFINE_BINARY_MP_MP(_mt, _dt, _mp, _mg, _op)                    \
  static Object prim_ ## _mt ## _ ## _op ## _bang (Object res, Object a, Object b) { \
    /* check */                                                         \
    if (!_mp(a)) {                                                      \
      marlais_error("%s: Wrong first argument", "%" #_mt "-" #_op);     \
    }                                                                   \
    if (!_mp(b)) {                                                      \
      marlais_error("%s: Wrong second argument", "%" #_mt "-" #_op);    \
    }                                                                   \
    /* operate */                                                       \
    _mt ## _ ## _op (_mg(res), _mg(a), _mg(b));                         \
    /* return */                                                        \
    return res;                                                         \
  }                                                                     \
  static Object prim_ ## _mt ## _ ## _op (Object a, Object b) {         \
    Object res;                                                         \
    /* allocate */                                                      \
    res = marlais_allocate_object(_dt, sizeof(_mt ## _obj));            \
    /* initialize */                                                    \
    _mt ## _init (_mg(res));                                            \
    /* operate and return */                                            \
    return prim_ ## _mt ## _ ## _op ## _bang (res, a, b);               \
  }

#define DEFINE_BINARY_MP_UI(_mt, _dt, _mp, _mg, _op)                    \
  static Object prim_ ## _mt ## _ ## _op ## _bang (Object res, Object a, Object b) { \
    /* check */                                                         \
    if (!_mp(a)) {                                                      \
      marlais_error("%s: Wrong first argument", "%" #_mt "-" #_op);     \
    }                                                                   \
    if (!UNSIGNEDP(b)) {                                                \
      marlais_error("%s: Wrong second argument", "%" #_mt "-" #_op);    \
    }                                                                   \
    /* operate */                                                       \
    _mt ## _ ## _op ## _ui (_mg(res), _mg(a), INTVAL(b));               \
    /* return */                                                        \
    return res;                                                         \
  }                                                                     \
  static Object prim_ ## _mt ## _ ## _op (Object a, Object b) {         \
    Object res;                                                         \
    /* allocate */                                                      \
    res = marlais_allocate_object(_dt, sizeof(_mt ## _obj));            \
    /* initialize */                                                    \
    _mt ## _init (_mg(res));                                            \
    /* operate and return */                                            \
    return prim_ ## _mt ## _ ## _op ## _bang (res, a, b);               \
  }

#define DEFINE_BINARY_MP_MPUI_COM(_mt, _dt, _mp, _mg, _op)              \
  static Object prim_ ## _mt ## _ ## _op ## _bang (Object res, Object a, Object b) { \
    Object m, i;                                                        \
    /* commutate */                                                     \
    if (_mp(a)) {                                                       \
      m = a;                                                            \
      i = b;                                                            \
    } else if (_mp(b)) {                                                \
      m = b;                                                            \
      i = a;                                                            \
    } else {                                                            \
      marlais_error("%s: Wrong arguments", "%" #_mt "-" #_op);          \
    }                                                                   \
    /* operate */                                                       \
    if (_mp(i)) {                                                       \
      _mt ## _ ## _op (_mg(res), _mg(m), _mg(i));                       \
    } else if (UNSIGNEDP(i)) {                                          \
      _mt ## _ ## _op ## _ui (_mg(res), _mg(m), INTVAL(i));             \
    } else {                                                            \
      marlais_error("%s: Wrong arguments", "%" #_mt "-" #_op);          \
    }                                                                   \
    /* return */                                                        \
    return res;                                                         \
  }                                                                     \
  static Object prim_ ## _mt ## _ ## _op (Object a, Object b) {         \
    Object res, m, i;                                                   \
    /* allocate */                                                      \
    res = marlais_allocate_object(_dt, sizeof(_mt ## _obj));            \
    /* initialize */                                                    \
    _mt ## _init (_mg(res));                                            \
    /* operate and return */                                            \
    return prim_ ## _mt ## _ ## _op ## _bang (res, a, b);               \
  }

#define DEFINE_BINARY_MP_MPUI(_mt, _dt, _mp, _mg, _op)                  \
  static Object prim_ ## _mt ## _ ## _op ## _bang (Object res, Object a, Object b) { \
    /* check */                                                         \
    if (!_mp(a)) {                                                      \
      marlais_error("%s: Wrong first argument", "%" #_mt "-" #_op);     \
    }                                                                   \
    /* operate */                                                       \
    if (_mp(b)) {                                                       \
      _mt ## _ ## _op (_mg(res), _mg(a), _mg(b));                       \
    } else if (UNSIGNEDP(b)) {                                          \
      _mt ## _ ## _op ## _ui (_mg(res), _mg(a), INTVAL(b));             \
    } else {                                                            \
      marlais_error("%s: Wrong second argument", "%" #_mt "-" #_op);    \
    }                                                                   \
    /* return */                                                        \
    return res;                                                         \
  }                                                                     \
  static Object prim_ ## _mt ## _ ## _op (Object a, Object b) {         \
    Object res;                                                         \
    /* allocate */                                                      \
    res = marlais_allocate_object(_dt, sizeof(_mt ## _obj));            \
    /* initialize */                                                    \
    _mt ## _init (_mg(res));                                            \
    /* operate and return */                                            \
    return prim_ ## _mt ## _ ## _op ## _bang (res, a, b);               \
  }

#define DEFINE_BINARY_MPUI_MPUI(_mt, _dt, _mp, _mg, _op)                \
  static Object prim_ ## _mt ## _ ## _op ## _bang (Object res, Object a, Object b) { \
    /* operate */                                                       \
    if (_mp(a) && _mp(b)) {                                             \
      _mt ## _ ## _op (_mg(res), _mg(a), _mg(b));                       \
    } else if (_mp(a)) {                                                \
      _mt ## _ui_ ## _op (_mg(res), INTVAL(a), _mg(b));                 \
    } else if (_mp(b)) {                                                \
      _mt ## _ ## _op ## _ui (_mg(res), _mg(a), INTVAL(b));             \
    } else {                                                            \
      marlais_error("%s: Wrong arguments", "%" #_mt "-" #_op);          \
    }                                                                   \
    /* return */                                                        \
    return res;                                                         \
  }                                                                     \
  static Object prim_ ## _mt ## _ ## _op (Object a, Object b) {         \
    Object res;                                                         \
    /* allocate */                                                      \
    res = marlais_allocate_object(_dt, sizeof(_mt ## _obj));            \
    /* initialize */                                                    \
    _mt ## _init (_mg(res));                                            \
    /* operate and return */                                            \
    return prim_ ## _mt ## _ ## _op ## _bang (res, a, b);               \
  }

DEFINE_COMPARE_MP_MP(mpf, MPFloat, MPFP, MPFVAL);
DEFINE_BINARY_MP_MPUI_COM(mpf, MPFloat, MPFP, MPFVAL, add);
DEFINE_BINARY_MPUI_MPUI(mpf, MPFloat, MPFP, MPFVAL, sub);
DEFINE_BINARY_MP_MPUI_COM(mpf, MPFloat, MPFP, MPFVAL, mul);
DEFINE_BINARY_MPUI_MPUI(mpf, MPFloat, MPFP, MPFVAL, div);
DEFINE_BINARY_MP_UI(mpf, MPFloat, MPFP, MPFVAL, pow);
DEFINE_UNARY_MP(mpf, MPFloat, MPFP, MPFVAL, neg);
DEFINE_UNARY_MP(mpf, MPFloat, MPFP, MPFVAL, abs);
DEFINE_UNARY_MP(mpf, MPFloat, MPFP, MPFVAL, ceil);
DEFINE_UNARY_MP(mpf, MPFloat, MPFP, MPFVAL, floor);
DEFINE_UNARY_MP(mpf, MPFloat, MPFP, MPFVAL, trunc);
DEFINE_UNARY_MP(mpf, MPFloat, MPFP, MPFVAL, sqrt);

static Object
prim_mpf_zero_p (Object a)
{
  return marlais_make_boolean(mpf_sgn(MPFVAL(a)) == 0);
}

static Object
prim_mpf_positive_p (Object a)
{
  return marlais_make_boolean(mpf_sgn(MPFVAL(a)) > 0);
}

static Object
prim_mpf_negative_p (Object a)
{
  return marlais_make_boolean(mpf_sgn(MPFVAL(a)) < 0);
}

DEFINE_COMPARE_MP_MP(mpq, MPRatio, MPQP, MPQVAL);
DEFINE_BINARY_MP_MP(mpq, MPRatio, MPQP, MPQVAL, add);
DEFINE_BINARY_MP_MP(mpq, MPRatio, MPQP, MPQVAL, sub);
DEFINE_BINARY_MP_MP(mpq, MPRatio, MPQP, MPQVAL, mul);
DEFINE_BINARY_MP_MP(mpq, MPRatio, MPQP, MPQVAL, div);
DEFINE_UNARY_MP(mpq, MPRatio, MPQP, MPQVAL, neg);
DEFINE_UNARY_MP(mpq, MPRatio, MPQP, MPQVAL, abs);
DEFINE_UNARY_MP(mpq, MPRatio, MPQP, MPQVAL, inv);

static Object
prim_mpq_zero_p (Object a)
{
  return marlais_make_boolean(mpq_sgn(MPQVAL(a)) == 0);
}

static Object
prim_mpq_positive_p (Object a)
{
  return marlais_make_boolean(mpq_sgn(MPQVAL(a)) > 0);
}

static Object
prim_mpq_negative_p (Object a)
{
  return marlais_make_boolean(mpq_sgn(MPQVAL(a)) < 0);
}

DEFINE_COMPARE_MP_MP(mpz, MPInteger, MPZP, MPZVAL);
DEFINE_BINARY_MP_MPUI_COM(mpz, MPInteger, MPZP, MPZVAL, add);
DEFINE_BINARY_MPUI_MPUI(mpz, MPInteger, MPZP, MPZVAL, sub);
DEFINE_BINARY_MP_MPUI_COM(mpz, MPInteger, MPZP, MPZVAL, mul);
DEFINE_BINARY_MP_MPUI(mpz, MPInteger, MPZP, MPZVAL, cdiv_q);
DEFINE_BINARY_MP_MPUI(mpz, MPInteger, MPZP, MPZVAL, cdiv_r);
DEFINE_BINARY_MP_MPUI(mpz, MPInteger, MPZP, MPZVAL, fdiv_q);
DEFINE_BINARY_MP_MPUI(mpz, MPInteger, MPZP, MPZVAL, fdiv_r);
DEFINE_BINARY_MP_MPUI(mpz, MPInteger, MPZP, MPZVAL, tdiv_q);
DEFINE_BINARY_MP_MPUI(mpz, MPInteger, MPZP, MPZVAL, tdiv_r);
DEFINE_UNARY_MP(mpz, MPInteger, MPZP, MPZVAL, neg);
DEFINE_UNARY_MP(mpz, MPInteger, MPZP, MPZVAL, abs);

static Object
prim_mpz_zero_p (Object a)
{
  return marlais_make_boolean(mpz_sgn(MPZVAL(a)) == 0);
}

static Object
prim_mpz_positive_p (Object a)
{
  return marlais_make_boolean(mpz_sgn(MPZVAL(a)) > 0);
}

static Object
prim_mpz_negative_p (Object a)
{
  return marlais_make_boolean(mpz_sgn(MPZVAL(a)) < 0);
}

static Object
prim_mpz_even_p (Object a)
{
  return marlais_make_boolean(mpz_even_p(MPZVAL(a)));
}

static Object
prim_mpz_odd_p (Object a)
{
  return marlais_make_boolean(mpz_odd_p(MPZVAL(a)));
}
