/*

   number.c

   This software is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this software; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Original copyright notice follows:

   Copyright, 1993, Brent Benson.  All Rights Reserved.
   0.4 & 0.5 Revisions Copyright 1994, Joseph N. Wilson.  All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#include <math.h>

#include "number.h"

#include "prim.h"
#include "values.h"

#ifdef BIG_INTEGERS
#include "biginteger.h"

#define MAX_SMALL_INT ((1 << 29) - 1)
#define MAX_SMALL_FACTOR (23170)

#define INT_ABS(i) (i < 0 ? -i : i)

#endif

#if defined(_CX_UX) || defined(MACOS) || defined(sgi) || defined(_HP_UX) \
|| defined(__hpux) || defined(__SunOS_5__) || defined(__linux__) \
|| defined(_WIN32) || defined(mips) || defined(__APPLE__)
#define NO_DOUBLE_INT_ARITH
#endif

#ifdef NO_DOUBLE_INT_ARITH
static double anint (double x);
static double aint (double x);

#endif

/* primitives */

static Object odd_p (Object n);
static Object even_p (Object n);
static Object int_zero_p (Object n);
static Object double_zero_p (Object n);
static Object int_positive_p (Object n);
static Object double_positive_p (Object n);
static Object int_negative_p (Object n);
static Object double_negative_p (Object n);
static Object integral_p (Object n);
static Object int_to_double (Object n);
static Object double_to_int (Object n);
static Object int_negative (Object n);
static Object double_negative (Object n);
static Object int_inverse (Object n);
static Object double_inverse (Object n);
static Object binary_int_plus (Object n1, Object n2);
static Object binary_int_minus (Object n1, Object n2);
static Object binary_int_times (Object n1, Object n2);
static Object binary_int_divide (Object n1, Object n2);
static Object binary_double_plus (Object n1, Object n2);
static Object binary_double_minus (Object n1, Object n2);
static Object binary_double_times (Object n1, Object n2);
static Object binary_double_divide (Object n1, Object n2);
static Object binary_less_than (Object n1, Object n2);
static Object int_sqrt (Object n);
static Object double_sqrt (Object n);
static Object int_abs (Object n);
static Object double_abs (Object n);
static Object int_quotient (Object n1, Object n2);
static Object ash (Object n, Object count);
static Object binary_logand (Object n1, Object n2);
static Object binary_logior (Object n1, Object n2);
static Object double_sin (Object n1);
static Object double_cos (Object n1);
static Object double_atan2 (Object n1, Object n2);
static Object floor_func (Object d);
static Object ceiling (Object d);
static Object round (Object d);
static Object truncate (Object d);
static Object modulo (Object d1, Object d2);
static Object modulo_double (Object d1, Object d2);
static Object remainder_double (Object d1, Object d2);
static Object remainder_int (Object i1, Object i2);
static Object double_exp (Object d);
static Object double_log (Object d);
static Object floor_divide (Object d1, Object d2);
static Object ceiling_divide (Object d1, Object d2);
static Object round_divide (Object d1, Object d2);
static Object truncate_divide (Object d1, Object d2);
static Object int_truncate_divide (Object i1, Object i2);
static struct primitive number_prims[] =
{
    {"%odd?", prim_1, odd_p},
    {"%even?", prim_1, even_p},
    {"%int-zero?", prim_1, int_zero_p},
    {"%double-zero?", prim_1, double_zero_p},
    {"%int-positive?", prim_1, int_positive_p},
    {"%double-positive?", prim_1, double_positive_p},
    {"%int-negative?", prim_1, int_negative_p},
    {"%double-negative?", prim_1, double_negative_p},
    {"%integral?", prim_1, integral_p},
    {"%int-to-double", prim_1, int_to_double},
    {"%double-to-int", prim_1, double_to_int},
    {"%int-negative", prim_1, int_negative},
    {"%double-negative", prim_1, double_negative},
    {"%int-inverse", prim_1, int_inverse},
    {"%double-inverse", prim_1, double_inverse},
    {"%binary-int+", prim_2, binary_int_plus},
    {"%binary-int-", prim_2, binary_int_minus},
    {"%binary-int*", prim_2, binary_int_times},
    {"%binary-int/", prim_2, binary_int_divide},
    {"%binary-double+", prim_2, binary_double_plus},
    {"%binary-double-", prim_2, binary_double_minus},
    {"%binary-double*", prim_2, binary_double_times},
    {"%binary-double/", prim_2, binary_double_divide},
    {"%binary-less-than", prim_2, binary_less_than},
    {"%int-sqrt", prim_1, int_sqrt},
    {"%double-sqrt", prim_1, double_sqrt},
    {"%int-abs", prim_1, int_abs},
    {"%double-abs", prim_1, double_abs},
    {"%quotient", prim_2, int_quotient},
    {"%ash", prim_2, ash},
    {"%sin", prim_1, double_sin},
    {"%cos", prim_1, double_cos},
    {"%atan2", prim_2, double_atan2},
    {"%binary-logand", prim_2, binary_logand},
    {"%binary-logior", prim_2, binary_logior},
    {"%floor", prim_1, floor_func},
    {"%ceiling", prim_1, ceiling},
    {"%round", prim_1, round},
    {"%truncate", prim_1, truncate},
    {"%modulo", prim_2, modulo},
    {"%modulo-double", prim_2, modulo_double},
    {"%remainder-double", prim_2, remainder_double},
    {"%remainder-int", prim_2, remainder_int},
    {"%exp", prim_1, double_exp},
    {"%ln", prim_1, double_log},
    {"%floor/", prim_2, floor_divide},
    {"%ceiling/", prim_2, ceiling_divide},
    {"%round/", prim_2, round_divide},
    {"%truncate/", prim_2, truncate_divide},
    {"%int-truncate/", prim_2, int_truncate_divide},
};

/* function definitions */

void
init_number_prims (void)
{
    int num;

    num = sizeof (number_prims) / sizeof (struct primitive);

    init_prims (num, number_prims);

#ifdef BIG_INTEGERS
    init_big_integer_prims ();
#endif
}

#ifdef BIG_INTEGERS

Object
make_integer (int i)
{
#ifndef SMALL_OBJECTS
    if (INT_ABS (i) <= MAX_SMALL_INT) {
	Object obj;

	obj = allocate_object (sizeof (struct object));

	TYPE (obj) = Integer;
	INTVAL (obj) = i;
	return (obj);
    } else
	return make_big_integer (i);
#else
    if (INT_ABS (i) <= MAX_SMALL_INT)
	return (MAKE_INT (i));
    else
	return make_big_integer (i);
#endif
}

#else

Object
make_integer (int i)
{
#ifndef SMALL_OBJECTS
    Object obj;

    obj = allocate_object (sizeof (struct object));

    TYPE (obj) = Integer;
    INTVAL (obj) = i;
    return (obj);
#else
    return (MAKE_INT (i));
#endif
}
#endif

Object
make_ratio (int numerator, int denominator)
{
    Object obj;

    obj = allocate_object (sizeof (struct ratio));

    RATIOTYPE (obj) = Ratio;
    RATIONUM (obj) = numerator;
    RATIODEN (obj) = denominator;
    return (obj);
}

Object
make_dfloat (double d)
{
    Object obj;

    obj = allocate_object (sizeof (struct double_float));

    DFLOATTYPE (obj) = DoubleFloat;
    DFLOATVAL (obj) = d;
    return (obj);
}

/* primitives */

static Object
odd_p (Object n)
{
    if ((INTVAL (n) % 2) == 1) {
	return (true_object);
    } else {
	return (false_object);
    }
}

static Object
even_p (Object n)
{
    if ((INTVAL (n) % 2) == 0) {
	return (true_object);
    } else {
	return (false_object);
    }
}

static Object
int_zero_p (Object n)
{
    if (INTVAL (n) == 0) {
	return (true_object);
    } else {
	return (false_object);
    }
}

static Object
double_zero_p (Object n)
{
    if (DFLOATVAL (n) == 0.0) {
	return (true_object);
    } else {
	return (false_object);
    }
}

static Object
int_positive_p (Object n)
{
    if (INTVAL (n) > 0) {
	return (true_object);
    } else {
	return (false_object);
    }
}

static Object
double_positive_p (Object n)
{
    if (DFLOATVAL (n) > 0.0) {
	return (true_object);
    } else {
	return (false_object);
    }
}

static Object
int_negative_p (Object n)
{
    if (INTVAL (n) < 0) {
	return (true_object);
    } else {
	return (false_object);
    }
}

static Object
double_negative_p (Object n)
{
    if (DFLOATVAL (n) < 0.0) {
	return (true_object);
    } else {
	return (false_object);
    }
}

static Object
integral_p (Object n)
{
    if (INTEGERP (n)) {
	return (true_object);
    } else {
	return (false_object);
    }
}

static Object
int_to_double (Object n)
{
    return (make_dfloat (INTVAL (n)));
}

static Object
double_to_int (Object n)
{
    return (make_integer (DFLOATVAL (n)));
}

static Object
int_negative (Object n)
{
    return (make_integer (-INTVAL (n)));
}

static Object
double_negative (Object n)
{
    return (make_dfloat (-DFLOATVAL (n)));
}

static Object
int_inverse (Object n)
{
    return (make_dfloat (1.0 / INTVAL (n)));
}

static Object
double_inverse (Object n)
{
    return (make_dfloat (1 / DFLOATVAL (n)));
}

static Object
binary_int_plus (Object n1, Object n2)
{
    return (make_integer (INTVAL (n1) + INTVAL (n2)));
}

static Object
binary_int_minus (Object n1, Object n2)
{
    return (make_integer (INTVAL (n1) - INTVAL (n2)));
}

#ifdef BIG_INTEGERS

static Object
binary_int_times (Object n1, Object n2)
{
    int i1 = INTVAL (n1), i2 = INTVAL (n2);

    // watch for overflow.
    if (i1 <= MAX_SMALL_FACTOR && i2 <= MAX_SMALL_FACTOR)
	return make_integer (i1 * i2);
    else
	return binary_bigint_times (make_big_integer (i1), make_big_integer (i2));
}

#else

static Object
binary_int_times (Object n1, Object n2)
{
    return (make_integer (INTVAL (n1) * INTVAL (n2)));
}

#endif

static Object
binary_int_divide (Object n1, Object n2)
{
    if ((INTVAL (n1) % INTVAL (n2)) == 0) {
	return (make_integer (INTVAL (n1) / INTVAL (n2)));
    } else {
	return (make_dfloat ((double) INTVAL (n1) / (double) INTVAL (n2)));
    }
}

static Object
binary_double_plus (Object n1, Object n2)
{
    return (make_dfloat (DFLOATVAL (n1) + DFLOATVAL (n2)));
}

static Object
binary_double_minus (Object n1, Object n2)
{
    return (make_dfloat (DFLOATVAL (n1) - DFLOATVAL (n2)));
}

static Object
binary_double_times (Object n1, Object n2)
{
    return (make_dfloat (DFLOATVAL (n1) * DFLOATVAL (n2)));
}

static Object
binary_double_divide (Object n1, Object n2)
{
    return (make_dfloat (DFLOATVAL (n1) / DFLOATVAL (n2)));
}

static Object
binary_less_than (Object n1, Object n2)
{
    if (INTEGERP (n1)) {
	if (INTEGERP (n2)) {
	    if (INTVAL (n1) < INTVAL (n2)) {
		return (true_object);
	    } else {
		return (false_object);
	    }
	} else {
	    if (INTVAL (n1) < DFLOATVAL (n2)) {
		return (true_object);
	    } else {
		return (false_object);
	    }
	}
    } else {
	if (INTEGERP (n2)) {
	    if (DFLOATVAL (n1) < INTVAL (n2)) {
		return (true_object);
	    } else {
		return (false_object);
	    }
	} else {
	    if (DFLOATVAL (n1) < DFLOATVAL (n2)) {
		return (true_object);
	    } else {
		return (false_object);
	    }
	}
    }
}

static Object
int_sqrt (Object n)
{
    double ans;

    ans = sqrt (INTVAL (n));
    if ((ans - floor (ans)) == 0) {
	return (make_integer (ans));
    } else {
	return (make_dfloat (ans));
    }
}

static Object
double_sqrt (Object n)
{
    return (make_dfloat (sqrt (DFLOATVAL (n))));
}

static Object
int_abs (Object n)
{
    int val;

    val = INTVAL (n);
    if (val < 0) {
	return (make_integer (-val));
    } else {
	return (n);
    }
}

static Object
double_abs (Object n)
{
    return (make_dfloat (fabs (DFLOATVAL (n))));
}

static Object
int_quotient (Object n1, Object n2)
{
    return (make_integer (INTVAL (n1) / INTVAL (n2)));
}

static Object
ash (Object n, Object count)
{
    int num;

    num = INTVAL (count);
    return (make_integer ((num > 0) ? (INTVAL (n) << num)
			  : (INTVAL (n) >> -num)));
}

static Object
double_sin (Object n1)
{
    return (make_dfloat (sin (DFLOATVAL (n1))));
}

static Object
double_cos (Object n1)
{
    return (make_dfloat (cos (DFLOATVAL (n1))));
}

static Object
double_atan2 (Object n1, Object n2)
{
    return (make_dfloat (atan2 (DFLOATVAL (n1), DFLOATVAL (n2))));
}

static Object
binary_logand (Object n1, Object n2)
{
    return (make_integer (INTVAL (n1) & INTVAL (n2)));
}

static Object
binary_logior (Object n1, Object n2)
{
    return (make_integer (INTVAL (n1) | INTVAL (n2)));
}

static Object
floor_func (Object d)
{
    double dval, tmp = floor (dval = DFLOATVAL (d));

    return construct_values (2,
			     make_integer ((int) tmp),
			     make_dfloat (dval - tmp));
}

static Object
ceiling (Object d)
{
    double dval, tmp = ceil (dval = DFLOATVAL (d));

    return construct_values (2,
			     make_integer ((int) tmp),
			     make_dfloat (dval - tmp));
}

static Object
round (Object d)
{
    double dval, tmp = anint (dval = DFLOATVAL (d));

    return construct_values (2,
			     make_integer ((int) tmp),
			     make_dfloat (dval - tmp));
}

static Object
truncate (Object d)
{
    double dval, tmp = aint (dval = DFLOATVAL (d));

    return construct_values (2,
			     make_integer ((int) tmp),
			     make_dfloat (dval - tmp));
}

#if 0
static int
sign (int x)
{
    return (x < 0 ? -1 : (x > 0 ? 1 : 0));
}

static Object
modulo (Object i1, Object i2)
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
    return make_integer (r);
}

#else

static Object
modulo (Object i1, Object i2)
{
    double d1val;
    double d2val;
    double tmp = (d1val = INTVAL (i1)) / (d2val = (float) INTVAL (i2));

    return make_integer ((int) (d1val - d2val * floor (tmp)));
}

#endif

static Object
modulo_double (Object d1, Object d2)
{
    double d1val;
    double d2val;
    double tmp = (d1val = DFLOATVAL (d1)) / (d2val = DFLOATVAL (d1));

    return make_dfloat (d1val - d2val * floor (tmp));
}

static Object
double_exp (Object n1)
{
    return (make_dfloat (exp (DFLOATVAL (n1))));
}

static Object
double_log (Object n1)
{
    return (make_dfloat (log (DFLOATVAL (n1))));
}

static Object
floor_divide (Object d1, Object d2)
{
    double d1val;
    double d2val;
    int intpart = floor ((d1val = DFLOATVAL (d1)) / (d2val = DFLOATVAL (d2)));

    return construct_values (2,
			     make_integer (intpart),
			     make_dfloat (d1val - d2val * intpart));
}

static Object
ceiling_divide (Object d1, Object d2)
{
    double d1val;
    double d2val;
    int intpart = ceil ((d1val = DFLOATVAL (d1)) / (d2val = DFLOATVAL (d2)));

    return construct_values (2,
			     make_integer (intpart),
			     make_dfloat (d1val - d2val * intpart));
}

static Object
round_divide (Object d1, Object d2)
{
    double d1val;
    double d2val;
    int intpart = anint ((d1val = DFLOATVAL (d1)) / (d2val = DFLOATVAL (d2)));

    return construct_values (2,
			     make_integer (intpart),
			     make_dfloat (d1val - d2val * intpart));
}

static Object
truncate_divide (Object d1, Object d2)
{
    double d1val;
    double d2val;
    int intpart = aint ((d1val = DFLOATVAL (d1)) / (d2val = DFLOATVAL (d2)));

    return construct_values (2,
			     make_integer (intpart),
			     make_dfloat (d1val - d2val * intpart));
}
static Object
int_truncate_divide (Object i1, Object i2)
{
    int i1val;
    int i2val;
    int quotient = (int) ((float) (i1val = INTVAL (i1)) / (i2val = INTVAL (i2)));

    return construct_values (2,
			     make_integer (quotient),
			     make_integer (i1val - i2val * quotient));
}

static Object
remainder_double (Object d1, Object d2)
{
    double d1val;
    double d2val;
    int intpart = aint ((d1val = DFLOATVAL (d1)) / (d2val = DFLOATVAL (d2)));

    return make_dfloat (d1val - d2val * intpart);
}

static Object
remainder_int (Object i1, Object i2)
{
    int i1val;
    int i2val;
    int quotient = (int) ((float) (i1val = INTVAL (i1)) / (i2val = INTVAL (i2)));

    return make_integer (i1val - i2val * quotient);
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
