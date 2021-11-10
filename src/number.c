/* number.c -- see COPYRIGHT for use */

#include <marlais/number.h>

#include <marlais/alloc.h>
#include <marlais/prim.h>
#include <marlais/values.h>

#ifdef MARLAIS_ENABLE_BIG_INTEGERS
#include "biginteger.h"
#endif

#include <math.h>

/* Helper macros */

#define MAX_SMALL_FACTOR (23170)
#define INT_ABS(i) (i < 0 ? -i : i)

/* Emulate anint and aint */

#if defined(_CX_UX) || defined(MACOS) || defined(sgi) || defined(_HP_UX) \
|| defined(__hpux) || defined(__SunOS_5__) || defined(__linux__) \
|| defined(_WIN32) || defined(mips) || defined(__APPLE__)
#define NO_DOUBLE_INT_ARITH
#endif

#ifdef NO_DOUBLE_INT_ARITH
static double anint (double x);
static double aint (double x);
#endif

/* Internal variables */

#ifndef MARLAIS_OBJECT_MODEL_SMALL
#if MARLAIS_CONFIG_INTEGER_CACHE > 0
static Object integer_cache[MARLAIS_CONFIG_INTEGER_CACHE];
#endif
#endif

#if MARLAIS_CONFIG_RATIO_CACHE > 0
static Object ratio_cache[MARLAIS_CONFIG_RATIO_CACHE^2];
#endif

static Object sfloat_zero;
static Object sfloat_one;

static Object dfloat_zero;
static Object dfloat_one;

/* Primitives */

static Object prim_odd_p (Object n);
static Object prim_even_p (Object n);
static Object prim_int_zero_p (Object n);
static Object prim_double_zero_p (Object n);
static Object prim_int_positive_p (Object n);
static Object prim_double_positive_p (Object n);
static Object prim_int_negative_p (Object n);
static Object prim_double_negative_p (Object n);
static Object prim_integral_p (Object n);
static Object prim_int_to_double (Object n);
static Object prim_double_to_int (Object n);
static Object prim_int_negative (Object n);
static Object prim_double_negative (Object n);
static Object prim_int_inverse (Object n);
static Object prim_double_inverse (Object n);
static Object prim_binary_int_plus (Object n1, Object n2);
static Object prim_binary_int_minus (Object n1, Object n2);
static Object prim_binary_int_times (Object n1, Object n2);
static Object prim_binary_int_divide (Object n1, Object n2);
static Object prim_binary_double_plus (Object n1, Object n2);
static Object prim_binary_double_minus (Object n1, Object n2);
static Object prim_binary_double_times (Object n1, Object n2);
static Object prim_binary_double_divide (Object n1, Object n2);
static Object prim_binary_less_than (Object n1, Object n2);
static Object prim_int_sqrt (Object n);
static Object prim_double_sqrt (Object n);
static Object prim_int_abs (Object n);
static Object prim_double_abs (Object n);
static Object prim_int_quotient (Object n1, Object n2);
static Object prim_ash (Object n, Object count);
static Object prim_binary_logand (Object n1, Object n2);
static Object prim_binary_logior (Object n1, Object n2);
static Object prim_double_sin (Object n1);
static Object prim_double_cos (Object n1);
static Object prim_double_atan2 (Object n1, Object n2);
static Object prim_floor_func (Object d);
static Object prim_ceiling (Object d);
static Object prim_round (Object d);
static Object prim_truncate (Object d);
static Object prim_modulo (Object d1, Object d2);
static Object prim_modulo_double (Object d1, Object d2);
static Object prim_remainder_double (Object d1, Object d2);
static Object prim_remainder_int (Object i1, Object i2);
static Object prim_double_exp (Object d);
static Object prim_double_log (Object d);
static Object prim_floor_divide (Object d1, Object d2);
static Object prim_ceiling_divide (Object d1, Object d2);
static Object prim_round_divide (Object d1, Object d2);
static Object prim_truncate_divide (Object d1, Object d2);
static Object prim_int_truncate_divide (Object i1, Object i2);

static struct primitive number_prims[] =
{
    {"%odd?", prim_1, prim_odd_p},
    {"%even?", prim_1, prim_even_p},
    {"%int-zero?", prim_1, prim_int_zero_p},
    {"%double-zero?", prim_1, prim_double_zero_p},
    {"%int-positive?", prim_1, prim_int_positive_p},
    {"%double-positive?", prim_1, prim_double_positive_p},
    {"%int-negative?", prim_1, prim_int_negative_p},
    {"%double-negative?", prim_1, prim_double_negative_p},
    {"%integral?", prim_1, prim_integral_p},
    {"%int-to-double", prim_1, prim_int_to_double},
    {"%double-to-int", prim_1, prim_double_to_int},
    {"%int-negative", prim_1, prim_int_negative},
    {"%double-negative", prim_1, prim_double_negative},
    {"%int-inverse", prim_1, prim_int_inverse},
    {"%double-inverse", prim_1, prim_double_inverse},
    {"%binary-int+", prim_2, prim_binary_int_plus},
    {"%binary-int-", prim_2, prim_binary_int_minus},
    {"%binary-int*", prim_2, prim_binary_int_times},
    {"%binary-int/", prim_2, prim_binary_int_divide},
    {"%binary-double+", prim_2, prim_binary_double_plus},
    {"%binary-double-", prim_2, prim_binary_double_minus},
    {"%binary-double*", prim_2, prim_binary_double_times},
    {"%binary-double/", prim_2, prim_binary_double_divide},
    {"%binary-less-than", prim_2, prim_binary_less_than},
    {"%int-sqrt", prim_1, prim_int_sqrt},
    {"%double-sqrt", prim_1, prim_double_sqrt},
    {"%int-abs", prim_1, prim_int_abs},
    {"%double-abs", prim_1, prim_double_abs},
    {"%quotient", prim_2, prim_int_quotient},
    {"%ash", prim_2, prim_ash},
    {"%double-sin", prim_1, prim_double_sin},
    {"%double-cos", prim_1, prim_double_cos},
    {"%double-atan2", prim_2, prim_double_atan2},
    {"%binary-logand", prim_2, prim_binary_logand},
    {"%binary-logior", prim_2, prim_binary_logior},
    {"%floor", prim_1, prim_floor_func},
    {"%ceiling", prim_1, prim_ceiling},
    {"%round", prim_1, prim_round},
    {"%truncate", prim_1, prim_truncate},
    {"%modulo", prim_2, prim_modulo},
    {"%modulo-double", prim_2, prim_modulo_double},
    {"%remainder-double", prim_2, prim_remainder_double},
    {"%remainder-int", prim_2, prim_remainder_int},
    {"%double-exp", prim_1, prim_double_exp},
    {"%double-ln", prim_1, prim_double_log},
    {"%floor/", prim_2, prim_floor_divide},
    {"%ceiling/", prim_2, prim_ceiling_divide},
    {"%round/", prim_2, prim_round_divide},
    {"%truncate/", prim_2, prim_truncate_divide},
    {"%int-truncate/", prim_2, prim_int_truncate_divide},
};

/* function definitions */

void
marlais_register_number (void)
{
    int num;

    num = sizeof (number_prims) / sizeof (struct primitive);

    marlais_register_prims (num, number_prims);

#ifdef MARLAIS_ENABLE_BIG_INTEGERS
    init_big_integer_prims ();
#endif
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

Object
marlais_make_sfloat (float f)
{
    Object obj;

    if(f == 0.0 && sfloat_zero) {
      return sfloat_zero;
    } else if (f == 1.0 && sfloat_one) {
      return sfloat_one;
    }

    obj = marlais_allocate_object (SingleFloat, sizeof (struct single_float));
    SFLOATVAL (obj) = f;

    if(f == 0.0) {
      sfloat_zero = obj;
    } else if (f == 1.0) {
      sfloat_one = obj;
    }

    return (obj);
}

Object
marlais_make_dfloat (double d)
{
    Object obj;

    if(d == 0.0 && dfloat_zero) {
      return dfloat_zero;
    } else if (d == 1.0 && dfloat_one) {
      return dfloat_one;
    }

    obj = marlais_allocate_object (DoubleFloat, sizeof (struct double_float));
    DFLOATVAL (obj) = d;

    if(d == 0.0) {
      dfloat_zero = obj;
    } else if (d == 1.0) {
      dfloat_one = obj;
    }

    return (obj);
}

/* Static functions */

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
prim_double_zero_p (Object n)
{
    if (DFLOATVAL (n) == 0.0) {
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
prim_double_positive_p (Object n)
{
    if (DFLOATVAL (n) > 0.0) {
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
prim_double_negative_p (Object n)
{
    if (DFLOATVAL (n) < 0.0) {
	return (MARLAIS_TRUE);
    } else {
	return (MARLAIS_FALSE);
    }
}

static Object
prim_integral_p (Object n)
{
    if (INTEGERP (n)) {
	return (MARLAIS_TRUE);
    } else {
	return (MARLAIS_FALSE);
    }
}

static Object
prim_int_to_double (Object n)
{
    return (marlais_make_dfloat (INTVAL (n)));
}

static Object
prim_double_to_int (Object n)
{
    return (marlais_make_integer (DFLOATVAL (n)));
}

static Object
prim_int_negative (Object n)
{
    return (marlais_make_integer (-INTVAL (n)));
}

static Object
prim_double_negative (Object n)
{
    return (marlais_make_dfloat (-DFLOATVAL (n)));
}

static Object
prim_int_inverse (Object n)
{
    return (marlais_make_dfloat (1.0 / INTVAL (n)));
}

static Object
prim_double_inverse (Object n)
{
    return (marlais_make_dfloat (1 / DFLOATVAL (n)));
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
prim_binary_double_plus (Object n1, Object n2)
{
    return (marlais_make_dfloat (DFLOATVAL (n1) + DFLOATVAL (n2)));
}

static Object
prim_binary_double_minus (Object n1, Object n2)
{
    return (marlais_make_dfloat (DFLOATVAL (n1) - DFLOATVAL (n2)));
}

static Object
prim_binary_double_times (Object n1, Object n2)
{
    return (marlais_make_dfloat (DFLOATVAL (n1) * DFLOATVAL (n2)));
}

static Object
prim_binary_double_divide (Object n1, Object n2)
{
    return (marlais_make_dfloat (DFLOATVAL (n1) / DFLOATVAL (n2)));
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
prim_double_sqrt (Object n)
{
    return (marlais_make_dfloat (sqrt (DFLOATVAL (n))));
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
prim_double_abs (Object n)
{
    return (marlais_make_dfloat (fabs (DFLOATVAL (n))));
}

static Object
prim_int_quotient (Object n1, Object n2)
{
    return (marlais_make_integer (INTVAL (n1) / INTVAL (n2)));
}

static Object
prim_ash (Object n, Object count)
{
    int num;

    num = INTVAL (count);
    return (marlais_make_integer ((num > 0) ? (INTVAL (n) << num)
			  : (INTVAL (n) >> -num)));
}

static Object
prim_double_sin (Object n1)
{
    return (marlais_make_dfloat (sin (DFLOATVAL (n1))));
}

static Object
prim_double_cos (Object n1)
{
    return (marlais_make_dfloat (cos (DFLOATVAL (n1))));
}

static Object
prim_double_atan2 (Object n1, Object n2)
{
    return (marlais_make_dfloat (atan2 (DFLOATVAL (n1), DFLOATVAL (n2))));
}

static Object
prim_binary_logand (Object n1, Object n2)
{
    return (marlais_make_integer (INTVAL (n1) & INTVAL (n2)));
}

static Object
prim_binary_logior (Object n1, Object n2)
{
    return (marlais_make_integer (INTVAL (n1) | INTVAL (n2)));
}

static Object
prim_floor_func (Object d)
{
    double dval, tmp = floor (dval = DFLOATVAL (d));

    return marlais_construct_values (2,
				     marlais_make_integer ((DyInteger) tmp),
				     marlais_make_dfloat (dval - tmp));
}

static Object
prim_ceiling (Object d)
{
    double dval, tmp = ceil (dval = DFLOATVAL (d));

    return marlais_construct_values (2,
				     marlais_make_integer ((DyInteger) tmp),
				     marlais_make_dfloat (dval - tmp));
}

static Object
prim_round (Object d)
{
    double dval, tmp = anint (dval = DFLOATVAL (d));

    return marlais_construct_values (2,
				     marlais_make_integer ((DyInteger) tmp),
				     marlais_make_dfloat (dval - tmp));
}

static Object
prim_truncate (Object d)
{
    double dval, tmp = aint (dval = DFLOATVAL (d));

    return marlais_construct_values (2,
				     marlais_make_integer ((DyInteger) tmp),
				     marlais_make_dfloat (dval - tmp));
}

#if 0
static int
prim_sign (int x)
{
    return (x < 0 ? -1 : (x > 0 ? 1 : 0));
}

static Object
prim_modulo (Object i1, Object i2)
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
prim_modulo (Object i1, Object i2)
{
    double d1val;
    double d2val;
    double tmp = (d1val = INTVAL (i1)) / (d2val = (float) INTVAL (i2));

    return marlais_make_integer ((DyInteger) (d1val - d2val * floor (tmp)));
}

#endif

static Object
prim_modulo_double (Object d1, Object d2)
{
    double d1val;
    double d2val;
    double tmp = (d1val = DFLOATVAL (d1)) / (d2val = DFLOATVAL (d1));

    return marlais_make_dfloat (d1val - d2val * floor (tmp));
}

static Object
prim_double_exp (Object n1)
{
    return (marlais_make_dfloat (exp (DFLOATVAL (n1))));
}

static Object
prim_double_log (Object n1)
{
    return (marlais_make_dfloat (log (DFLOATVAL (n1))));
}

static Object
prim_floor_divide (Object d1, Object d2)
{
    double d1val;
    double d2val;
    int intpart = floor ((d1val = DFLOATVAL (d1)) / (d2val = DFLOATVAL (d2)));

    return marlais_construct_values (2,
				     marlais_make_integer (intpart),
				     marlais_make_dfloat (d1val - d2val * intpart));
}

static Object
prim_ceiling_divide (Object d1, Object d2)
{
    double d1val;
    double d2val;
    int intpart = ceil ((d1val = DFLOATVAL (d1)) / (d2val = DFLOATVAL (d2)));

    return marlais_construct_values (2,
				     marlais_make_integer (intpart),
				     marlais_make_dfloat (d1val - d2val * intpart));
}

static Object
prim_round_divide (Object d1, Object d2)
{
    double d1val;
    double d2val;
    int intpart = anint ((d1val = DFLOATVAL (d1)) / (d2val = DFLOATVAL (d2)));

    return marlais_construct_values (2,
				     marlais_make_integer (intpart),
				     marlais_make_dfloat (d1val - d2val * intpart));
}

static Object
prim_truncate_divide (Object d1, Object d2)
{
    double d1val;
    double d2val;
    int intpart = aint ((d1val = DFLOATVAL (d1)) / (d2val = DFLOATVAL (d2)));

    return marlais_construct_values (2,
				     marlais_make_integer (intpart),
				     marlais_make_dfloat (d1val - d2val * intpart));
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

static Object
prim_remainder_double (Object d1, Object d2)
{
    double d1val;
    double d2val;
    int intpart = aint ((d1val = DFLOATVAL (d1)) / (d2val = DFLOATVAL (d2)));

    return marlais_make_dfloat (d1val - d2val * intpart);
}

static Object
prim_remainder_int (Object i1, Object i2)
{
    int i1val;
    int i2val;
    int quotient = (int) ((float) (i1val = INTVAL (i1)) / (i2val = INTVAL (i2)));

    return marlais_make_integer (i1val - i2val * quotient);
}

#ifdef NO_DOUBLE_INT_ARITH
static double
anint (double x)
{
    double y;

    if (x >= 0) {
	return ((modf (x + 0.5, &y) < 0) ? (y - 1) : y);
    } else {
	return (-((modf (0.5 - x, &y) < 0) ? (y - 1) : y));
    }
}

static double
aint (double x)
{
    double y;

    if (x > 0) {
	return ((modf (x, &y) < 0) ? (y - 1) : y);
    } else {
	return (-((modf (-x, &y) < 0) ? (y - 1) : y));
    }
}
#endif
