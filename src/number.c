/* number.c -- see COPYRIGHT for use */

#include <marlais/number.h>

#include <marlais/alloc.h>
#include <marlais/env.h>
#include <marlais/prim.h>
#include <marlais/symbol.h>
#include <marlais/values.h>

#include <float.h>
#include <math.h>

/* Helper macros */

#define MAX_SMALL_FACTOR (23170)
#define INT_ABS(i) (i < 0 ? -i : i)

/* Internal variables */

/* Cache for <small-integer> */
#ifndef MARLAIS_OBJECT_MODEL_SMALL
#if MARLAIS_CONFIG_INTEGER_CACHE > 0
static Object integer_cache[MARLAIS_CONFIG_INTEGER_CACHE];
#endif
#endif

/* Cache for <ratio> */
#if MARLAIS_CONFIG_RATIO_CACHE > 0
static Object ratio_cache[MARLAIS_CONFIG_RATIO_CACHE^2];
#endif

/* Primitives */

static Object prim_odd_p (Object n);
static Object prim_even_p (Object n);
static Object prim_int_zero_p (Object n);
static Object prim_int_positive_p (Object n);
static Object prim_int_negative_p (Object n);
static Object prim_int_negative (Object n);
static Object prim_int_inverse (Object n);
static Object prim_binary_int_plus (Object n1, Object n2);
static Object prim_binary_int_minus (Object n1, Object n2);
static Object prim_binary_int_times (Object n1, Object n2);
static Object prim_binary_int_divide (Object n1, Object n2);
static Object prim_binary_less_than (Object n1, Object n2);
static Object prim_int_sqrt (Object n);
static Object prim_int_abs (Object n);
static Object prim_int_quotient (Object n1, Object n2);
static Object prim_int_ash (Object n, Object count);
static Object prim_int_lognot (Object n1);
static Object prim_int_logand (Object n1, Object n2);
static Object prim_int_logior (Object n1, Object n2);
static Object prim_int_logxor (Object n1, Object n2);
static Object prim_int_modulo (Object d1, Object d2);
static Object prim_int_remainder (Object i1, Object i2);
static Object prim_int_truncate_divide (Object i1, Object i2);

static struct primitive number_prims[] =
{
    {"%int-odd?", prim_1, prim_odd_p},
    {"%int-even?", prim_1, prim_even_p},
    {"%int-zero?", prim_1, prim_int_zero_p},
    {"%int-positive?", prim_1, prim_int_positive_p},
    {"%int-negative?", prim_1, prim_int_negative_p},
    {"%int-negative", prim_1, prim_int_negative},
    {"%int-inverse", prim_1, prim_int_inverse},
    {"%int+", prim_2, prim_binary_int_plus},
    {"%int-", prim_2, prim_binary_int_minus},
    {"%int*", prim_2, prim_binary_int_times},
    {"%int/", prim_2, prim_binary_int_divide},
    {"%int<", prim_2, prim_binary_less_than},
    {"%int-sqrt", prim_1, prim_int_sqrt},
    {"%int-abs", prim_1, prim_int_abs},
    {"%int-quotient", prim_2, prim_int_quotient},
    {"%int-ash", prim_2, prim_int_ash},
    {"%int-lognot", prim_1, prim_int_lognot},
    {"%int-logand", prim_2, prim_int_logand},
    {"%int-logior", prim_2, prim_int_logior},
    {"%int-logxor", prim_2, prim_int_logxor},
    {"%int-modulo", prim_2, prim_int_modulo},
    {"%int-remainder", prim_2, prim_int_remainder},
    {"%int-truncate/", prim_2, prim_int_truncate_divide},
};

/* function definitions */

void
marlais_register_number (void)
{
    /* register primitives */
    int num =  sizeof (number_prims) / sizeof (struct primitive);
    marlais_register_prims (num, number_prims);

    /* integer constants */
    marlais_add_export (marlais_make_name ("$integer-bits"),
                        marlais_make_integer (MARLAIS_INTEGER_WIDTH), 1);
    marlais_add_export (marlais_make_name ("$integer-size"),
                        marlais_make_integer (sizeof(DyInteger)), 1);
    marlais_add_export (marlais_make_name ("$integer-significant-bits"),
                        marlais_make_integer (MARLAIS_INTEGER_WIDTH - 1), 1);
    marlais_add_export (marlais_make_name ("$minimum-integer"),
                        marlais_make_integer (MARLAIS_INTEGER_MIN), 1);
    marlais_add_export (marlais_make_name ("$maximum-integer"),
                        marlais_make_integer (MARLAIS_INTEGER_MAX), 1);
}

#ifndef MARLAIS_OBJECT_MODEL_SMALL
/* small version is inline in marlais/number.h */
Object
marlais_make_integer (DyInteger i)
{
  Object obj;

#if MARLAIS_CONFIG_INTEGER_CACHE > 0
  if(i >= 0 && i < MARLAIS_CONFIG_INTEGER_CACHE) {
    if(integer_cache[i] != NULL) {
      return integer_cache[i];
    }
  }
#endif

  obj = marlais_allocate_object (Integer, sizeof (struct integer));
  INTVAL (obj) = i;

#if MARLAIS_CONFIG_INTEGER_CACHE > 0
  if(i >= 0 && i < MARLAIS_CONFIG_INTEGER_CACHE) {
    integer_cache[i] = obj;
  }
#endif

  return (obj);
}
#endif

Object
marlais_make_ratio (DyInteger numerator, DyInteger denominator)
{
    Object obj;

#if MARLAIS_CONFIG_RATIO_CACHE > 0
    DyInteger index = -1;
    if(numerator >= 0 && numerator < MARLAIS_CONFIG_RATIO_CACHE
       && denominator >= 0 && denominator < MARLAIS_CONFIG_RATIO_CACHE) {
      index = numerator + denominator * MARLAIS_CONFIG_RATIO_CACHE;
      if(ratio_cache[index]) {
        return ratio_cache[index];
      }
    }
#endif

    obj = marlais_allocate_object (Ratio, sizeof (struct ratio));
    RATIONUM (obj) = numerator;
    RATIODEN (obj) = denominator;

#if MARLAIS_CONFIG_RATIO_CACHE > 0
    if(index != -1) {
      ratio_cache[index] = obj;
    }
#endif

    return (obj);
}

/* Primitives */

static Object
prim_odd_p (Object n)
{
    if ((INTVAL (n) % 2) == 1) {
        return (MARLAIS_TRUE);
    } else {
        return (MARLAIS_FALSE);
    }
}

static Object
prim_even_p (Object n)
{
    if ((INTVAL (n) % 2) == 0) {
        return (MARLAIS_TRUE);
    } else {
        return (MARLAIS_FALSE);
    }
}

static Object
prim_int_zero_p (Object n)
{
    if (INTVAL (n) == 0) {
        return (MARLAIS_TRUE);
    } else {
        return (MARLAIS_FALSE);
    }
}

static Object
prim_int_positive_p (Object n)
{
    if (INTVAL (n) > 0) {
        return (MARLAIS_TRUE);
    } else {
        return (MARLAIS_FALSE);
    }
}

static Object
prim_int_negative_p (Object n)
{
    if (INTVAL (n) < 0) {
        return (MARLAIS_TRUE);
    } else {
        return (MARLAIS_FALSE);
    }
}

static Object
prim_int_negative (Object n)
{
    return (marlais_make_integer (-INTVAL (n)));
}

static Object
prim_int_inverse (Object n)
{
    return (marlais_make_dfloat (1.0 / INTVAL (n)));
}

static Object
prim_binary_int_plus (Object n1, Object n2)
{
    return (marlais_make_integer (INTVAL (n1) + INTVAL (n2)));
}

static Object
prim_binary_int_minus (Object n1, Object n2)
{
    return (marlais_make_integer (INTVAL (n1) - INTVAL (n2)));
}

#ifdef MARLAIS_ENABLE_BIG_INTEGERS

static Object
prim_binary_int_times (Object n1, Object n2)
{
    int i1 = INTVAL (n1), i2 = INTVAL (n2);

    // watch for overflow.
    if (i1 <= MAX_SMALL_FACTOR && i2 <= MAX_SMALL_FACTOR)
	return marlais_make_integer (i1 * i2);
    else
	return binary_bigint_times (make_big_integer (i1), make_big_integer (i2));
}

#else

static Object
prim_binary_int_times (Object n1, Object n2)
{
    return (marlais_make_integer (INTVAL (n1) * INTVAL (n2)));
}

#endif

static Object
prim_binary_int_divide (Object n1, Object n2)
{
    if ((INTVAL (n1) % INTVAL (n2)) == 0) {
        return (marlais_make_integer (INTVAL (n1) / INTVAL (n2)));
    } else {
        return (marlais_make_dfloat ((double) INTVAL (n1) / (double) INTVAL (n2)));
    }
}

static Object
prim_binary_less_than (Object n1, Object n2)
{
    if (INTEGERP (n1)) {
        if (INTEGERP (n2)) {
            if (INTVAL (n1) < INTVAL (n2)) {
                return (MARLAIS_TRUE);
            } else {
                return (MARLAIS_FALSE);
            }
        } else {
            if (INTVAL (n1) < DFLOATVAL (n2)) {
                return (MARLAIS_TRUE);
            } else {
                return (MARLAIS_FALSE);
            }
        }
    } else {
        if (INTEGERP (n2)) {
            if (DFLOATVAL (n1) < INTVAL (n2)) {
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
prim_int_sqrt (Object n)
{
    double ans;

    ans = sqrt (INTVAL (n));
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

    val = INTVAL (n);
    if (val < 0) {
        return (marlais_make_integer (-val));
    } else {
        return (n);
    }
}

static Object
prim_int_quotient (Object n1, Object n2)
{
    return (marlais_make_integer (INTVAL (n1) / INTVAL (n2)));
}

static Object
prim_int_ash (Object n, Object count)
{
    int num;

    num = INTVAL (count);
    return (marlais_make_integer ((num > 0) ? (INTVAL (n) << num)
                          : (INTVAL (n) >> -num)));
}

static Object
prim_int_lognot (Object n1)
{
    return (marlais_make_integer (~INTVAL (n1)));
}

static Object
prim_int_logand (Object n1, Object n2)
{
    return (marlais_make_integer (INTVAL (n1) & INTVAL (n2)));
}

static Object
prim_int_logior (Object n1, Object n2)
{
    return (marlais_make_integer (INTVAL (n1) | INTVAL (n2)));
}

static Object
prim_int_logxor (Object n1, Object n2)
{
    return (marlais_make_integer (INTVAL (n1) ^ INTVAL (n2)));
}

#if 0
static int
prim_sign (int x)
{
    return (x < 0 ? -1 : (x > 0 ? 1 : 0));
}

static Object
prim_int_modulo (Object i1, Object i2)
{
    int i1val = INTVAL (i1);
    int i2val = INTVAL (i2);
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
prim_int_modulo (Object i1, Object i2)
{
    double d1val;
    double d2val;
    double tmp = (d1val = INTVAL (i1)) / (d2val = (float) INTVAL (i2));

    return marlais_make_integer ((DyInteger) (d1val - d2val * floor (tmp)));
}

#endif

static Object
prim_int_remainder (Object i1, Object i2)
{
    int i1val;
    int i2val;
    int quotient = (int) ((float) (i1val = INTVAL (i1)) / (i2val = INTVAL (i2)));

    return marlais_make_integer (i1val - i2val * quotient);
}

static Object
prim_int_truncate_divide (Object i1, Object i2)
{
    int i1val;
    int i2val;
    int quotient = (int) ((float) (i1val = INTVAL (i1)) / (i2val = INTVAL (i2)));

    return marlais_construct_values (2,
                                     marlais_make_integer (quotient),
                                     marlais_make_integer (i1val - i2val * quotient));
}
