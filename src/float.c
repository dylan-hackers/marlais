
#include <marlais/float.h>

#include <marlais/alloc.h>
#include <marlais/env.h>
#include <marlais/prim.h>
#include <marlais/symbol.h>
#include <marlais/values.h>

#include <float.h>
#include <math.h>

/* Configuration */

/* Emulate anint and aint (where are they not emulated!?) */
#if defined(_CX_UX) || defined(MACOS) || defined(sgi) || defined(_HP_UX) \
|| defined(__hpux) || defined(__SunOS_5__) || defined(__linux__) \
|| defined(_WIN32) || defined(mips) || defined(__APPLE__)
#define NO_DOUBLE_INT_ARITH
#endif

/* Internal functions */

#ifdef NO_DOUBLE_INT_ARITH
static double anint (double x);
static double aint (double x);
#endif

/* Internal variables */

/* Cache for <single-float> */
static Object sfloat_zero;
static Object sfloat_one;

/* Cache for <double-float> */
static Object dfloat_zero;
static Object dfloat_one;

#ifdef MARLAIS_ENABLE_EXTENDED_FLOAT
/* Cache for <extended-float> */
static Object efloat_zero;
static Object efloat_one;
#endif

/* Primitives */

static Object prim_int_to_double (Object n);
static Object prim_double_to_int (Object n);

static Object prim_double_zero_p (Object n);
static Object prim_double_positive_p (Object n);
static Object prim_double_negative_p (Object n);

static Object prim_double_negative (Object n);
static Object prim_double_inverse (Object n);

static Object prim_binary_double_plus (Object n1, Object n2);
static Object prim_binary_double_minus (Object n1, Object n2);
static Object prim_binary_double_times (Object n1, Object n2);
static Object prim_binary_double_divide (Object n1, Object n2);

static Object prim_modulo_double (Object d1, Object d2);
static Object prim_remainder_double (Object d1, Object d2);

static Object prim_floor (Object d);
static Object prim_ceiling (Object d);
static Object prim_round (Object d);
static Object prim_truncate (Object d);

static Object prim_floor_divide (Object d1, Object d2);
static Object prim_ceiling_divide (Object d1, Object d2);
static Object prim_round_divide (Object d1, Object d2);
static Object prim_truncate_divide (Object d1, Object d2);

static Object prim_double_exp (Object d);
static Object prim_double_log (Object d);
static Object prim_double_sqrt (Object n);
static Object prim_double_abs (Object n);
static Object prim_double_sin (Object n1);
static Object prim_double_cos (Object n1);
static Object prim_double_atan2 (Object n1, Object n2);

static struct primitive float_prims[] =
{
    {"%int-to-double", prim_1, prim_int_to_double},
    {"%double-to-int", prim_1, prim_double_to_int},

    {"%double-zero?", prim_1, prim_double_zero_p},
    {"%double-positive?", prim_1, prim_double_positive_p},
    {"%double-negative?", prim_1, prim_double_negative_p},

    {"%double-negative", prim_1, prim_double_negative},
    {"%double-inverse", prim_1, prim_double_inverse},

    {"%binary-double+", prim_2, prim_binary_double_plus},
    {"%binary-double-", prim_2, prim_binary_double_minus},
    {"%binary-double*", prim_2, prim_binary_double_times},
    {"%binary-double/", prim_2, prim_binary_double_divide},

    {"%modulo-double", prim_2, prim_modulo_double},
    {"%remainder-double", prim_2, prim_remainder_double},

    {"%floor", prim_1, prim_floor},
    {"%ceiling", prim_1, prim_ceiling},
    {"%round", prim_1, prim_round},
    {"%truncate", prim_1, prim_truncate},

    {"%floor/", prim_2, prim_floor_divide},
    {"%ceiling/", prim_2, prim_ceiling_divide},
    {"%round/", prim_2, prim_round_divide},
    {"%truncate/", prim_2, prim_truncate_divide},

    {"%double-exp", prim_1, prim_double_exp},
    {"%double-ln", prim_1, prim_double_log},
    {"%double-sqrt", prim_1, prim_double_sqrt},
    {"%double-abs", prim_1, prim_double_abs},
    {"%double-sin", prim_1, prim_double_sin},
    {"%double-cos", prim_1, prim_double_cos},
    {"%double-atan2", prim_2, prim_double_atan2},
};

/* External functions */

void
marlais_register_float (void)
{
    /* register primitives */
    int num =  sizeof (float_prims) / sizeof (struct primitive);
    marlais_register_prims (num, float_prims);

    /* float constants */
    marlais_add_export (marlais_make_name ("$float-radix"),
                        marlais_make_integer (FLT_RADIX), 1);

    /* single float */
    marlais_add_export (marlais_make_name ("$single-float-bits"),
                        marlais_make_integer (sizeof(float) * 8), 1);
    marlais_add_export (marlais_make_name ("$single-float-size"),
                        marlais_make_integer (sizeof(float)), 1);
#if 0
    /* TODO glibc on my machine has 0.0 as a value for this!? */
    marlais_add_export (marlais_make_name ("$single-float-epsilon"),
                        marlais_make_sfloat (FLT_EPSILON), 1);
#endif
    marlais_add_export (marlais_make_name ("$single-float-exponent-bits"),
                        marlais_make_integer (FLT_DIG), 1);
    marlais_add_export (marlais_make_name ("$minimum-single-float"),
                        marlais_make_sfloat (FLT_MIN), 1);
    marlais_add_export (marlais_make_name ("$maximum-single-float"),
                        marlais_make_sfloat (FLT_MAX), 1);
    marlais_add_export (marlais_make_name ("$minimum-single-float-exponent"),
                        marlais_make_integer (FLT_MIN_EXP), 1);
    marlais_add_export (marlais_make_name ("$maximum-single-float-exponent"),
                        marlais_make_integer (FLT_MAX_EXP), 1);

    /* double float */
    marlais_add_export (marlais_make_name ("$double-float-bits"),
                        marlais_make_integer (sizeof(double) * 8), 1);
    marlais_add_export (marlais_make_name ("$double-float-size"),
                        marlais_make_integer (sizeof(double)), 1);
#if 0
    /* TODO glibc on my machine has 0.0 as a value for this!? */
    marlais_add_export (marlais_make_name ("$double-float-epsilon"),
                        marlais_make_dfloat (DBL_EPSILON), 1);
#endif
    marlais_add_export (marlais_make_name ("$double-float-exponent-bits"),
                        marlais_make_integer (DBL_DIG), 1);
    marlais_add_export (marlais_make_name ("$minimum-double-float"),
                        marlais_make_dfloat (DBL_MIN), 1);
    marlais_add_export (marlais_make_name ("$maximum-double-float"),
                        marlais_make_dfloat (DBL_MAX), 1);
    marlais_add_export (marlais_make_name ("$minimum-double-float-exponent"),
                        marlais_make_integer (DBL_MIN_EXP), 1);
    marlais_add_export (marlais_make_name ("$maximum-double-float-exponent"),
                        marlais_make_integer (DBL_MAX_EXP), 1);
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

#ifdef MARLAIS_ENABLE_EXTENDED_FLOAT
Object
marlais_make_efloat (long double e)
{
    Object obj;

    if(e == 0.0 && efloat_zero) {
      return efloat_zero;
    } else if (e == 1.0 && efloat_one) {
      return efloat_one;
    }

    obj = marlais_allocate_object (ExtendedFloat, sizeof (struct extended_float));
    EFLOATVAL (obj) = e;

    if(e == 0.0) {
      efloat_zero = obj;
    } else if (e == 1.0) {
      efloat_one = obj;
    }

    return (obj);
}
#endif

/* Internal functions */

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

/* Primitives */

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
prim_double_zero_p (Object n)
{
    if (DFLOATVAL (n) == 0.0) {
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
prim_double_negative_p (Object n)
{
    if (DFLOATVAL (n) < 0.0) {
        return (MARLAIS_TRUE);
    } else {
        return (MARLAIS_FALSE);
    }
}

static Object
prim_double_negative (Object n)
{
    return (marlais_make_dfloat (-DFLOATVAL (n)));
}

static Object
prim_double_inverse (Object n)
{
    return (marlais_make_dfloat (1 / DFLOATVAL (n)));
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
prim_modulo_double (Object d1, Object d2)
{
    double d1val;
    double d2val;
    double tmp = (d1val = DFLOATVAL (d1)) / (d2val = DFLOATVAL (d1));

    return marlais_make_dfloat (d1val - d2val * floor (tmp));
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
prim_floor (Object d)
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
prim_double_sqrt (Object n)
{
    return (marlais_make_dfloat (sqrt (DFLOATVAL (n))));
}

static Object
prim_double_abs (Object n)
{
    return (marlais_make_dfloat (fabs (DFLOATVAL (n))));
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