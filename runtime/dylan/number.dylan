module: dylan

//
// numbers.dylan
//
// Authors:
//   Brent Benson
//

//
// Generic functions
//

// Numeric predicates

define generic odd? (n :: <integer>) => value :: <boolean>;
define generic even? (n :: <integer>) => value :: <boolean>;
define generic zero? (n :: <object>) => value :: <boolean>;
define generic positive? (r :: <object>) => value :: <boolean>;
define generic negative? (r :: <object>) => value :: <boolean>;
define generic integral? (n :: <object>) => value :: <boolean>;

// Extended predicates

define generic nan? (n :: <object>) => value :: <boolean>;
define generic finite? (n :: <object>) => value :: <boolean>;
define generic infinite? (n :: <object>) => value :: <boolean>;

// Sign operations

define generic abs (n :: <number>) => value :: <number>;
define generic negative (n :: <number>) => value :: <number>;

// Arithmetic operations

define generic \+ (n1 :: <number>, n2 :: <number>) => value :: <number>;
define generic \* (n1 :: <number>, n2 :: <number>) => value :: <number>;
define generic \- (n1 :: <number>, n2 :: <number>) => value :: <number>;
define generic \/ (n1 :: <number>, n2 :: <number>) => value :: <number>;

// Division operations

define generic modulo (r1 :: <real>, r2 :: <real>) => modulus :: <real>;
define generic remainder (r1 :: <real>, r2 :: <real>) => remains :: <real>;

define generic floor (r :: <real>)
 => (integer-part :: <integer>, remainder :: <real>);
define generic ceiling (r :: <real>)
 => (integer-part :: <integer>, remainder :: <real>);
define generic round (r :: <real>)
 => (integer-part :: <integer>, remainder :: <real>);
define generic truncate (r :: <real>)
 => (integer-part :: <integer>, remainder :: <real>);

define generic floor/ (r1 :: <real>, r2 :: <real>)
 => (quotient :: <integer>, remainder :: <real>);
define generic ceiling/ (r1 :: <real>, r2 :: <real>)
 => (quotient :: <integer>, remainder :: <real>);
define generic round/ (r1 :: <real>, r2 :: <real>)
 => (quotient :: <integer>, remainder :: <real>);
define generic truncate/ (r1 :: <real>, r2 :: <real>)
 => (quotient :: <integer>, remainder :: <real>);

// Integer operations

define generic ash (i :: <integer>, count :: <integer>) => value :: <integer>;

define generic quotient (i1 :: <integer>, i2 :: <integer>) => value :: <integer>;

define generic lognot (i :: <integer>) => value :: <integer>;

define generic binary-logand (i1 :: <integer>, i2 :: <integer>) => value :: <integer>;
define generic binary-logior (i1 :: <integer>, i2 :: <integer>) => value :: <integer>;
define generic binary-logxor (i1 :: <integer>, i2 :: <integer>) => value :: <integer>;

define method logand (i1 :: <integer>, #rest more-integers)
  %list-reduce (binary-logand, i1, more-integers);
end method logand;

define method logior (i1 :: <integer>, #rest more-integers)
  %list-reduce (binary-logior, i1, more-integers);
end method logior;

define method logxor (i1 :: <integer>, #rest more-integers)
  %list-reduce (binary-logxor, i1, more-integers);
end method logand;

//
// Exponention, logarithms, roots, transcendentals
//

define generic \^ (n :: <number>, i :: <integer>) => value :: <number>;

define generic exp (n :: <number>) => value :: <number>;
define generic ln (n :: <number>) => value :: <number>;

define generic sqrt (n :: <number>) => value :: <number>;

define generic cos (n :: <number>) => value :: <number>;
define generic sin (n :: <number>) => value :: <number>;
define generic tan (n :: <number>) => value :: <number>;
define generic atan (n :: <number>) => value :: <number>;
define generic atan2 (n1 :: <number>, n2 :: <number>) => value :: <number>;

//
// Number comparison
//

// Simple numbers with themselves

define method \< (n1 :: <small-integer>, n2 :: <small-integer>)
  %int< (n1, n2);
end method \<;

define method \< (n1 :: <double-float>, n2 :: <double-float>)
  %double< (n1, n2);
end method \<;

// <double-float> and small integers

define method \= (i :: <small-integer>, d :: <double-float>)
  as(<double-float>, i) == d;
end method \=;

define method \= (d :: <double-float>, i :: <small-integer>)
  d == as(<double-float>, i);
end method \=;

//
// Number coercion
//

// <double-float> and small integers

define method as (df-class == <double-float>, i :: <small-integer>)
  %int-to-double(i);
end method as;

define method as (i-class == <small-integer>, df :: <double-float>)
  %double-to-int(df);
end method as;

//
// Numeric predicates
//

// odd?

define method odd? (i :: <small-integer>) => value :: <boolean>;
  %int-odd?(i);
end method odd?;

// even?

define method even? (i :: <small-integer>) => value :: <boolean>;
  %int-even?(i);
end method even?;

// zero?

define method zero? (i :: <small-integer>) => value :: <boolean>;
  %int-zero?(i);
end method zero?;

define method zero? (d :: <double-float>) => value :: <boolean>;
  %double-zero?(d);
end method zero?;

// positive?

define method positive? (i :: <small-integer>) => value :: <boolean>;
  %int-positive?(i);
end method positive?;

define method positive? (d :: <double-float>) => value :: <boolean>;
  %double-positive?(d);
end method positive?;

// negative?

define method negative? (i :: <small-integer>) => value :: <boolean>;
  %int-negative?(i);
end method negavite?;

define method negative? (d :: <double-float>) => value :: <boolean>;
  %double-negative?(d);
end method negative?;

// integral?

define method integral? (n :: <object>) => value :: <boolean>;
  #f;
end method integral?;

define method integral? (i :: <integer>) => value :: <boolean>;
  #t;
end method integral?;

//
// Extended predicates
//

// On <number>

define method nan? (n :: <number>)
 => (value :: <boolean>);
  #f
end method;

define method finite? (n :: <number>)
 => (value :: <boolean>);
  #t
end method;

define method infinite? (n :: <number>)
 => (value :: <boolean>);
  #f
end method;

// On <double-float>

define method nan? (n :: <double-float>)
 => (value :: <boolean>);
  %double-nan? (n);
end method;

define method finite? (n :: <double-float>)
 => (value :: <boolean>);
  %double-finite? (n);
end method;

define method infinite? (n :: <double-float>)
 => (value :: <boolean>);
  %double-infinite? (n);
end method;

//
// Sign operations
//

define method abs (i :: <small-integer>)
  %int-abs(i);
end method abs;

define method abs (d :: <double-float>)
  %double-abs(d);
end method abs;

define method negative (i :: <small-integer>) => value :: <small-integer>;
  %int-negative (i);
end method negative;

define method negative (d :: <double-float>) => value :: <double-float>;
  %double-negative (d);
end method negative;

//
// Arithmetic operations
//

// Binary <small-integer> <small-integer>

define method \+ (i1 :: <small-integer>, i2 :: <small-integer>)
  %int-add (i1, i2);
end method \+;

define method \- (i1 :: <small-integer>, i2 :: <small-integer>)
  %int-sub (i1, i2);
end method \-;

define method \* (i1 :: <small-integer>, i2 :: <small-integer>)
  %int-mul (i1, i2);
end method \*;

define method \/ (i1 :: <small-integer>, i2 :: <small-integer>)
  %int-div (i1, i2);
end method \/;

// Binary <double-float> <double-float>

define method \+ (d1 :: <double-float>, d2 :: <double-float>)
  %double-add (d1, d2);
end method \+;

define method \- (d1 :: <double-float>, d2 :: <double-float>)
  %double-sub (d1, d2);
end method \-;

define method \* (d1 :: <double-float>, d2 :: <double-float>)
  %double-mul (d1, d2);
end method \*;

define method \/ (d1 :: <double-float>, d2 :: <double-float>)
  %double-div (d1, d2);
end method \/;

// Binary <small-integer> <double-float>

define method \+ (i1 :: <small-integer>, d2 :: <double-float>)
  %double-add (as (<double-float>, i1), d2);
end method \+;

define method \- (i1 :: <small-integer>, d2 :: <double-float>)
  %double-sub (as (<double-float>, i1), d2);
end method \-;

define method \* (i1 :: <small-integer>, d2 :: <double-float>)
  %double-mul (as (<double-float>, i1), d2);
end method \*;

define method \/ (i1 :: <small-integer>, d2 :: <double-float>)
  %double-div (as (<double-float>, i1), d2);
end method \/;

// Binary <double-float> <small-integer>

define method \+ (d1 :: <double-float>, i2 :: <small-integer>)
  %double-add (d1, as (<double-float>, i2));
end method \+;

define method \- (d1 :: <double-float>, i2 :: <small-integer>)
  %double-sub (d1, as (<double-float>, i2));
end method \-;

define method \* (d1 :: <double-float>, i2 :: <small-integer>)
  %double-mul (d1, as (<double-float>, i2));
end method \*;

define method \/ (d1 :: <double-float>, i2 :: <small-integer>)
  %double-div (d1, as (<double-float>, i2));
end method \/;

//
// Division
//

// modulo

define method modulo (n1 :: <real>, n2 :: <real>)
 => modulus :: <real>;
  %double-mod ( as (<double-float>, n1), as (<double-float>, n2));
end method modulo;

define method modulo (i1 :: <small-integer>, i2 :: <small-integer>)
 => modulus :: <integer>;
  %int-mod (i1, i2);
end method modulo;

define method modulo (n1 :: <double-float>, n2 :: <double-float>)
 => modulus :: <double-float>;
  %double-mod (n1, n2);
end method modulo;

// remainder

define method remainder (n1 :: <real>, n2 :: <real>)
 => remainder :: <real>;
  %double-rem (as (<double-float>, n1), as (<double-float>, n2));
end method remainder;

define method remainder (i1 :: <small-integer>, i2 :: <small-integer>)
 => remainder :: <integer>;
  %int-rem (i1, i2);
end method remainder;

define method remainder (n1 :: <double-float>, n2 :: <double-float>)
 => remainder :: <double-float>;
  %double-rem (n1, n2);
end method remainder;

// floor

define method floor (n :: <real>)
 => (integer-part :: <integer>, remainder :: <real>);
  %double-floor (as (<double-float>, n));
end method;

define method floor (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %double-floor (d);
end method floor;

// ceiling

define method ceiling (n :: <real>)
 => (integer-part :: <integer>, remainder :: <real>);
  %double-ceiling (as (<double-float>, n));
end method;

define method ceiling (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %double-ceiling (d);
end method ceiling;

// round

define method round (n :: <real>)
 => (integer-part :: <integer>, remainder :: <real>);
  %double-round (as (<double-float>, n));
end method;

define method round (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %double-round (d);
end method round;

// truncate

define method truncate (n :: <real>)
 => (integer-part :: <integer>, remainder :: <real>);
  %double-round (as (<double-float>, n));
end method;

define method truncate (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %double-truncate (d);
end method truncate;

// floor/

define method floor/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %double-floor/ (as (<double-float>, n1), as (<double-float>, n2));
end method floor/;

define method floor/ (n1 :: <double-float>, n2 :: <double-float>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %double-floor/ (n1, n2);
end method floor/;

// ceiling/

define method ceiling/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %double-ceiling/ (as (<double-float>, n1), as (<double-float>, n2));
end method ceiling/;

define method ceiling/ (n1 :: <double-float>, n2 :: <double-float>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %double-ceiling/ (n1, n2);
end method ceiling/;

// round/

define method round/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %double-round/ (as (<double-float>, n1), as (<double-float>, n2));
end method round/;

define method round/ (n1 :: <double-float>, n2 :: <double-float>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %double-round/ (n1, n2);
end method round/;

// truncate/

define method truncate/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %double-truncate/ (as (<double-float>, n1), as (<double-float>, n2));
end method truncate/;

define method truncate/ (i1 :: <small-integer>, i2 :: <small-integer>)
 => (quotient :: <integer>, remainder :: <small-integer>);
  %int-truncate/ (i1, i2);
end method truncate/;

define method truncate/ (n1 :: <double-float>, n2 :: <double-float>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %double-truncate/ (n1, n2);
end method truncate/;

//
// Integer operations
//

define method ash (i :: <small-integer>, count :: <small-integer>)
  %int-ash (i, count);
end method ash;

define method quotient (i1 :: <small-integer>, i2 :: <small-integer>)
  %int-quotient (i1, i2);
end method quotient;

define method lognot (i :: <small-integer>)
 => value :: <small-integer>;
  %int-lognot (i);
end method;

define method binary-logand (i1 :: <integer>, i2 :: <integer>)
 => value :: <small-integer>;
  %int-logand (i1, i2);
end method;

define method binary-logior (i1 :: <small-integer>, i2 :: <small-integer>)
 => value :: <small-integer>;
  %int-logior (i1, i2);
end method;

define method binary-logxor (i1 :: <small-integer>, i2 :: <small-integer>)
 => value :: <small-integer>;
  %int-logxor (i1, i2);
end method;

//
// Exponention, logarithms, roots, transcendentals
//

// Thanks for Marty Hall for making the following get into this file
//
// Exponentiation has the following caveats:
//
// If there's no big integer support on your platform, this will
// give wrong results without warning for larger numbers.
//
// Raising floats to large exponents gives results that are
// limited by the precision of the float implementation.

define method \^ (base :: <number>, exponent :: <integer>)
 => base-to-exponent :: <number>;
  local method power(b :: <number>,
		     e :: <integer>,
		     residual :: <number>) => b-to-e :: <number>;
	  if (e == 0)
	    residual;
	  elseif (e.even?)
	    power (b * b, floor/(e, 2), residual);
	  else
	    power (b, e - 1, residual * b);
	  end if;
	end method power;

  if (exponent.negative?)
    1.0 / power (base, - exponent, 1);
  else
    power (base, exponent, 1);
  end if;
end method \^;

define method sqrt (i :: <small-integer>) => value :: <number>;
  %int-sqrt(i);
end method sqrt;

define method sqrt (d :: <double-float>) => value :: <number>;
  %double-sqrt(d);
end method sqrt;

define method exp (n :: <number>) => value :: <number>;
  %double-exp (as(<double-float>, n));
end method exp;

define method ln (n :: <number>) => value :: <number>;
  %double-ln (as(<double-float>, n));
end method ln;

define method cos (n :: <real>) => value :: <real>;
  %double-cos (as (<double-float>, n));
end method cos;

define method sin (n :: <real>) => value :: <real>;
  %double-sin (as (<double-float>, n));
end method sin;

define method tan (n :: <real>) => value :: <real>;
  %double-tan (as (<double-float>, n));
end method sin;

define method atan (n :: <real>) => value :: <real>;
  %double-atan (as (<double-float>, n));
end method atan2;

define method atan2 (n1 :: <real>, n2 :: <real>) => value :: <real>;
  %double-atan2 (as (<double-float>, n1), as (<double-float>, n2));
end method atan2;

//
// Minimum and maximum
//

define method max (n1 :: <real>, #rest more-reals) => value :: <real>;
  reduce (method (x, y) if (x < y) y else x end if; end,
	  n1,
	  more-reals);
end method max;

define method min (n1 :: <real>, #rest more-reals) => value :: <real>;
  reduce (method (x, y) if (x < y) x else y end if; end,
	  n1,
	  more-reals);
end method min;

//
// Least common multiple
//

// lcm -- 06/20/95 Marty Hall

define method lcm(int1 :: <integer>, int2 :: <integer>)
 => multiple :: <integer>;
  let result = floor/ (abs(int1 * int2), gcd(int1, int2));
  result;
end method lcm;

//
// Greatest common divisor
//

// GCD via Euclid's Algorithm, yielding O(log N) performance, where N is the
// smaller of the two numbers. See _Structure and Interpretation
// of Computer Programs_ pg 44. Note that having an extra case where
// int1 == int2 is not worth it, since then you have an additional test
// in the 95% of cases when int1 and int2 are distinct. This way, you have
// an extra function call in the 5% of cases when int1 is == to int2.
// 6/95 Marty Hall

define method gcd(int1 :: <integer>, int2 :: <integer>)
 => divisor :: <integer>;
  local method gcd-internal(larger :: <integer>, smaller :: <integer>)
	 => divisor :: <integer>;
	  if (smaller == 0)
	    larger;
	  else
	    gcd-internal(smaller, remainder(larger, smaller));
	  end if;
	end method gcd-internal;
  let int1 = abs(int1);
  let int2 = abs(int2);
  if (int1 > int2)
    gcd-internal(int1, int2);
  else
    gcd-internal(int2, int1);
  end if;
end method gcd;
