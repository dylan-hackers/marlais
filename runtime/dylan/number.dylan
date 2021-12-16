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

// Basic operations

define generic \+ (n1 :: <number>, n2 :: <number>) => value :: <number>;
define generic \* (n1 :: <number>, n2 :: <number>) => value :: <number>;
define generic \- (n1 :: <number>, n2 :: <number>) => value :: <number>;
define generic \/ (n1 :: <number>, n2 :: <number>) => value :: <number>;

define generic abs (n :: <number>) => value :: <number>;
define generic negative (n :: <number>) => value :: <number>;

// Division

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

define generic modulo (r1 :: <real>, r2 :: <real>) => modulus :: <real>;
define generic remainder (r1 :: <real>, r2 :: <real>) => remains :: <real>;

define generic \^ (n :: <number>, i :: <integer>) => value :: <number>;

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
// Arithmetic operations
//

// Unary

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

// Operations specific to integers

define method quotient (i1 :: <integer>, i2 :: <integer>)
  %int-quotient (i1, i2);
end method quotient;

define method ash (i :: <small-integer>, count :: <small-integer>)
  %int-ash (i, count);
end method ash;

// TODO generic function, bignum support
define method lognot (i :: <small-integer>)
  %int-lognot (i);
end method logior;

// TODO type safety, bignum support
define method logior (#rest integers)
  reduce1 (%int-logior, integers);
end method logior;

// TODO type safety, bignum support
define method logand (#rest integers)
  reduce1 (%int-logand, integers);
end method logand;

// TODO type safety, bignum support
define method logxor (#rest integers)
  reduce1 (%int-logxor, integers);
end method logand;

// Binary <small-integer> <small-integer>

define method \+ (i1 :: <small-integer>, i2 :: <small-integer>)
  %int+ (i1, i2);
end method \+;

define method \- (i1 :: <small-integer>, i2 :: <small-integer>)
  %int- (i1, i2);
end method \-;

define method \* (i1 ::<small-integer>, i2 :: <small-integer>)
  %int* (i1, i2);
end method \*;

define method \/ (i1 :: <small-integer>, i2 :: <small-integer>)
  %int/ (i1, i2);
end method \*;

// Binary <double-float> <double-float>

define method \+ (d1 :: <double-float>, d2 :: <double-float>)
  %double+ (d1, d2);
end method \+;

define method \- (d1 :: <double-float>, d2 :: <double-float>)
  %double- (d1, d2);
end method \-;

define method \* (d1 :: <double-float>, d2 :: <double-float>)
  %double* (d1, d2);
end method \*;

define method \/ (d1 :: <double-float>, d2 :: <double-float>)
  %double/ (d1, d2);
end method \/;

// Binary <small-integer> <double-float>

define method \+ (i1 :: <small-integer>, d2 :: <double-float>)
  %double+ (as (<double-float>, i1), d2);
end method \+;

define method \- (i1 :: <small-integer>, d2 :: <double-float>)
  %double- (as (<double-float>, i1), d2);
end method \-;

define method \* (i1 :: <small-integer>, d2 :: <double-float>)
  %double* (as (<double-float>, i1), d2);
end method \*;

define method \/ (i1 :: <small-integer>, d2 :: <double-float>)
  %double/ (as (<double-float>, i1), d2);
end method \/;

// Binary <double-float> <small-integer>

define method \+ (d1 :: <double-float>, i2 :: <small-integer>)
  %double+ (d1, as (<double-float>, i2));
end method \+;

define method \- (d1 :: <double-float>, i2 :: <small-integer>)
  %double- (d1, as (<double-float>, i2));
end method \-;

define method \* (d1 :: <double-float>, i2 :: <small-integer>)
  %double* (d1, as (<double-float>, i2));
end method \*;

define method \/ (d1 :: <double-float>, i2 :: <small-integer>)
  %double/ (d1, as (<double-float>, i2));
end method \/;

//
// Division
//

define method floor (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %double-floor (d);
end method floor;

define method ceiling (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %double-ceiling (d);
end method ceiling;

define method round (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %double-round (d);
end method round;

define method truncate (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %double-truncate (d);
end method truncate;

// TODO fix specialization
define method floor/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %floor/ (as (<double-float>, n1), as (<double-float>, n2));
end method floor/;

// TODO fix specialization
define method ceiling/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %ceiling/ (as (<double-float>, n1), as (<double-float>, n2));
end method ceiling/;

// TODO fix specialization
define method round/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %round/ (as (<double-float>, n1), as (<double-float>, n2));
end method round/;

// TODO fix specialization
define method truncate/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %truncate/ (as (<double-float>, n1), as (<double-float>, n2));
end method truncate/;

define method truncate/ (i1 :: <small-integer>, i2 :: <small-integer>)
 => (quotient :: <small-integer>, remainder :: <small-integer>);
  %int-truncate/ (i1, i2);
end method truncate/;

// TODO fix specialization
define method modulo (n1 :: <real>, n2 :: <real>)
 => modulus :: <real>;
  %double-modulo ( as (<double-float>, n1), as (<double-float>, n2));
end method modulo;

define method modulo (i1 :: <small-integer>, i2 :: <small-integer>)
 => modulus :: <small-integer>;
  %int-modulo (i1, i2);
end method modulo;

// TODO fix specialization
define method remainder (n1 :: <real>, n2 :: <real>)
 => remainder :: <real>;
  %double-remainder (as (<double-float>, n1), as (<double-float>, n2));
end method remainder;

define method remainder (i1 :: <small-integer>, i2 :: <small-integer>)
 => remainder :: <small-integer>;
  %int-remainder (i1, i2);
end method remainder;

//
// Minimum and maximum
//

define method max (n1 :: <real>, #rest more-reals)
  reduce (method (x, y) if (x < y) y else x end if; end,
	  n1,
	  more-reals);
end method max;

define method min (n1 :: <real>, #rest more-reals)
  reduce (method (x, y) if (x < y) x else y end if; end,
	  n1,
	  more-reals);
end method min;

//
// Exponentiation
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

//
// Various extensions
//

define method sqrt (i :: <small-integer>)
  %int-sqrt(i);
end method sqrt;

define method sqrt (d :: <double-float>)
  %double-sqrt(d);
end method sqrt;

// TODO fix specialization
define method sin (n :: <number>) => value :: <number>;
  %double-sin (as (<double-float>, n));
end method sin;

// TODO fix specialization
define method cos (n :: <number>) => value :: <number>;
  %double-cos (as (<double-float>, n));
end method cos;

// TODO fix specialization
define method atan2 (d1 :: <number>, d2 :: <number>)
  %double-atan2 (as (<double-float>, d1), as (<double-float>, d2));
end method atan2;

// TODO fix specialization
define method exp (n :: <number>)
  %double-exp (as(<double-float>, n));
end method exp;

// TODO fix specialization
define method ln (n :: <number>)
  %double-ln (as(<double-float>, n));
end method ln;

//
// Derived numeric functions
//

// lcm -- 06/20/95 Marty Hall

define method lcm(int1 :: <integer>, int2 :: <integer>)
 => multiple :: <integer>;
  let result = floor/ (abs(int1 * int2), gcd(int1, int2));
  result;
 end method lcm;

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
