/* number.c -- see COPYRIGHT for use */

#include <marlais/object/integer.h>

#include <marlais/core/env.h>
#include <marlais/object/prim.h>

#include <float.h>
#include <math.h>

/* Internal variables */

/* Cache for <small-integer> */
#ifndef MARLAIS_OBJECT_MODEL_TAGGED
#if MARLAIS_CONFIG_INTEGER_CACHE > 0
static Object integer_cache[MARLAIS_CONFIG_INTEGER_CACHE];
#endif
#endif

/* Primitives */

static Object prim_int_less_than (Object n1, Object n2);
static Object prim_int_odd_p (Object n);
static Object prim_int_even_p (Object n);
static Object prim_int_zero_p (Object n);
static Object prim_int_positive_p (Object n);
static Object prim_int_negative_p (Object n);
static Object prim_int_negative (Object n);
static Object prim_int_inverse (Object n);
static Object prim_int_add (Object n1, Object n2);
static Object prim_int_sub (Object n1, Object n2);
static Object prim_int_mul (Object n1, Object n2);
static Object prim_int_div (Object n1, Object n2);
static Object prim_int_rem (Object i1, Object i2);
static Object prim_int_mod (Object d1, Object d2);
static Object prim_int_truncate_divide (Object i1, Object i2);
static Object prim_int_abs (Object n);
static Object prim_int_quotient (Object n1, Object n2);
static Object prim_int_ash (Object n, Object count);
static Object prim_int_lognot (Object n1);
static Object prim_int_logand (Object n1, Object n2);
static Object prim_int_logior (Object n1, Object n2);
static Object prim_int_logxor (Object n1, Object n2);
static Object prim_int_sqrt (Object n);
static Object prim_int_cbrt (Object n);

static struct primitive integer_prims[] = {
  {"%int<", prim_2, prim_int_less_than},

  {"%int-odd?", prim_1, prim_int_odd_p},
  {"%int-even?", prim_1, prim_int_even_p},
  {"%int-zero?", prim_1, prim_int_zero_p},
  {"%int-positive?", prim_1, prim_int_positive_p},
  {"%int-negative?", prim_1, prim_int_negative_p},

  {"%int-abs",      prim_1, prim_int_abs},
  {"%int-negative", prim_1, prim_int_negative},
  {"%int-inverse",  prim_1, prim_int_inverse},

  {"%int-add", prim_2, prim_int_add},
  {"%int-sub", prim_2, prim_int_sub},
  {"%int-mul", prim_2, prim_int_mul},

  {"%int-div", prim_2, prim_int_div},
  {"%int-mod", prim_2, prim_int_mod},
  {"%int-rem", prim_2, prim_int_rem},
  {"%int-truncate/", prim_2, prim_int_truncate_divide},
  {"%int-quotient", prim_2, prim_int_quotient},

  {"%int-ash",    prim_2, prim_int_ash},
  {"%int-lognot", prim_1, prim_int_lognot},
  {"%int-logand", prim_2, prim_int_logand},
  {"%int-logior", prim_2, prim_int_logior},
  {"%int-logxor", prim_2, prim_int_logxor},

  {"%int-sqrt", prim_1, prim_int_sqrt},
  {"%int-cbrt", prim_1, prim_int_cbrt},
};

/* function definitions */

void
marlais_register_integer (void)
{
  /* register primitives */
  MARLAIS_REGISTER_PRIMS (integer_prims);

  /* integer constants */
  marlais_add_export (marlais_make_name ("$integer-bits"),
                      marlais_make_integer (MARLAIS_INTEGER_WIDTH), 1);
  marlais_add_export (marlais_make_name ("$integer-size"),
                      marlais_make_integer (sizeof(marlais_int_t)), 1);
  marlais_add_export (marlais_make_name ("$integer-significant-bits"),
                      marlais_make_integer (MARLAIS_INTEGER_WIDTH - 1), 1);
  marlais_add_export (marlais_make_name ("$minimum-integer"),
                      marlais_make_integer (MARLAIS_INTEGER_MIN), 1);
  marlais_add_export (marlais_make_name ("$maximum-integer"),
                      marlais_make_integer (MARLAIS_INTEGER_MAX), 1);
}

/* Primitives */

static Object
prim_int_less_than (Object n1, Object n2)
{
    if (marlais_is_integer_p (n1)) {
        if (marlais_is_integer_p (n2)) {
            if (marlais_get_int (n1) < marlais_get_int (n2)) {
                return (MARLAIS_TRUE);
            } else {
                return (MARLAIS_FALSE);
            }
        } else {
            if (marlais_get_int (n1) < DFLOATVAL (n2)) {
                return (MARLAIS_TRUE);
            } else {
                return (MARLAIS_FALSE);
            }
        }
    } else {
        if (marlais_is_integer_p (n2)) {
            if (DFLOATVAL (n1) < marlais_get_int (n2)) {
                return (MARLAIS_TRUE);
            } else {
                return (MARLAIS_FALSE);
            }
        } else {
            if (DFLOATVAL (n1) < DFLOATVAL (n2)) {
                return (MARLAIS_TRUE);
            } else {
                return (MARLAIS_FALSE);
            }
        }
    }
}

static Object
prim_int_odd_p (Object n)
{
    if ((marlais_get_int (n) % 2) == 1) {
        return (MARLAIS_TRUE);
    } else {
        return (MARLAIS_FALSE);
    }
}

static Object
prim_int_even_p (Object n)
{
    if ((marlais_get_int (n) % 2) == 0) {
        return (MARLAIS_TRUE);
    } else {
        return (MARLAIS_FALSE);
    }
}

static Object
prim_int_zero_p (Object n)
{
    if (marlais_get_int (n) == 0) {
        return (MARLAIS_TRUE);
    } else {
        return (MARLAIS_FALSE);
    }
}

static Object
prim_int_positive_p (Object n)
{
    if (marlais_get_int (n) > 0) {
        return (MARLAIS_TRUE);
    } else {
        return (MARLAIS_FALSE);
    }
}

static Object
prim_int_negative_p (Object n)
{
    if (marlais_get_int (n) < 0) {
        return (MARLAIS_TRUE);
    } else {
        return (MARLAIS_FALSE);
    }
}

static Object
prim_int_negative (Object n)
{
    return marlais_make_integer (-marlais_get_int (n));
}

static Object
prim_int_inverse (Object n)
{
    return marlais_make_dfloat (1.0 / marlais_get_int (n));
}

static Object
prim_int_add (Object n1, Object n2)
{
    return marlais_make_integer (marlais_get_int (n1) + marlais_get_int (n2));
}

static Object
prim_int_sub (Object n1, Object n2)
{
    return marlais_make_integer (marlais_get_int (n1) - marlais_get_int (n2));
}

static Object
prim_int_mul (Object n1, Object n2)
{
    return marlais_make_integer (marlais_get_int (n1) * marlais_get_int (n2));
}

static Object
prim_int_div (Object n1, Object n2)
{
    return marlais_make_integer (marlais_get_int (n1) / marlais_get_int (n2));
}

static Object
prim_int_sqrt (Object n)
{
    double ans;

    ans = sqrt (marlais_get_int (n));
    if ((ans - floor (ans)) == 0) {
        return (marlais_make_integer (ans));
    } else {
        return (marlais_make_dfloat (ans));
    }
}

static Object
prim_int_cbrt (Object n)
{
    double ans;

    ans = cbrt (marlais_get_int (n));
    if ((ans - floor (ans)) == 0) {
        return (marlais_make_integer (ans));
    } else {
        return (marlais_make_dfloat (ans));
    }
}

static Object
prim_int_abs (Object n)
{
    int val;

    val = marlais_get_int (n);
    if (val < 0) {
        return (marlais_make_integer (-val));
    } else {
        return (n);
    }
}

static Object
prim_int_quotient (Object n1, Object n2)
{
    return (marlais_make_integer (marlais_get_int (n1) / marlais_get_int (n2)));
}

static Object
prim_int_ash (Object n, Object count)
{
    int num;

    num = marlais_get_int (count);
    return (marlais_make_integer ((num > 0) ? (marlais_get_int (n) << num)
                          : (marlais_get_int (n) >> -num)));
}

static Object
prim_int_lognot (Object n1)
{
    return (marlais_make_integer (~marlais_get_int (n1)));
}

static Object
prim_int_logand (Object n1, Object n2)
{
    return (marlais_make_integer (marlais_get_int (n1) & marlais_get_int (n2)));
}

static Object
prim_int_logior (Object n1, Object n2)
{
    return (marlais_make_integer (marlais_get_int (n1) | marlais_get_int (n2)));
}

static Object
prim_int_logxor (Object n1, Object n2)
{
    return (marlais_make_integer (marlais_get_int (n1) ^ marlais_get_int (n2)));
}

#if 0
static int
prim_sign (int x)
{
    return (x < 0 ? -1 : (x > 0 ? 1 : 0));
}

static Object
prim_mod (Object i1, Object i2)
{
    int i1val = marlais_get_int (i1);
    int i2val = marlais_get_int (i2);
    int q = (i1val / i2val);
    int r = (i1val - i2val * q);

    if (r != 0) {
        if (sign (i1val) != sign (i2val)) {
            r += i2val;
        }
    }
    return marlais_make_integer (r);
}

#else

static Object
prim_int_mod (Object i1, Object i2)
{
    double d1val;
    double d2val;
    double tmp = (d1val = marlais_get_int (i1)) / (d2val = (float) marlais_get_int (i2));

    return marlais_make_integer ((marlais_int_t) (d1val - d2val * floor (tmp)));
}

#endif

static Object
prim_int_rem (Object i1, Object i2)
{
    int i1val;
    int i2val;
    int quotient = (int) ((float) (i1val = marlais_get_int (i1)) / (i2val = marlais_get_int (i2)));

    return marlais_make_integer (i1val - i2val * quotient);
}

static Object
prim_int_truncate_divide (Object i1, Object i2)
{
    int i1val;
    int i2val;
    int quotient = (int) ((float) (i1val = marlais_get_int (i1)) / (i2val = marlais_get_int (i2)));

    return marlais_values_args (2,
                                     marlais_make_integer (quotient),
                                     marlais_make_integer (i1val - i2val * quotient));
}
