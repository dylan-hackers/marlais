module: dylan

//
// numbers
//

//
// number.dyl - generic functions on numbers
//
// Brent Benson
//

//
// misc
//
define method odd? (i :: <small-integer>) => value :: <boolean>;
  %odd?(i);
end method odd?;

define method odd? (bi :: <big-integer>) => value :: <boolean>;
  %bigint-odd?(bi);
end method odd?;

define method even? (i :: <small-integer>) => value :: <boolean>;
  %even?(i);
end method even?;

define method even? (bi :: <big-integer>) => value :: <boolean>;
  %bigint-even?(bi);
end method even?;

define generic zero? (n :: <number>) => value :: <boolean>;

define method zero? (i :: <small-integer>) => value :: <boolean>;
  %int-zero?(i);
end method zero?;

define method zero? (bi :: <big-integer>) => value :: <boolean>;
  %bigint-zero?(bi);
end method zero?;

define method zero? (d :: <double-float>) => value :: <boolean>;
  %double-zero?(d);
end method zero?;

define generic positive? (r :: <real>) => value :: <boolean>;

define method positive? (i :: <small-integer>) => value :: <boolean>;
  %int-positive?(i);
end method positive?;

define method positive? (bi :: <big-integer>) => value :: <boolean>;
  %bigint-positive?(bi);
end method positive?;

define method positive? (d :: <double-float>) => value :: <boolean>;
  %double-positive?(d);
end method positive?;

define generic negative? (r :: <real>) => value :: <boolean>;

define method negative? (i :: <small-integer>) => value :: <boolean>;
  %int-negative?(i);
end method negavite?;

define method negative? (bi :: <big-integer>) => value :: <boolean>;
  %bigint-negative?(bi);
end method negavite?;

define method negative? (d :: <double-float>) => value :: <boolean>;
  %double-negative?(d);
end method negative?;

define method integral? (n :: <number>)
  #f;
end method integral?;

define method integral? (i :: <integer>)
  #t;
end method integral?;

// define method quotient (i1 :: <integer>, i2 :: <integer>)
//   %quotient?(i1, i2);
// end method quotient;

//
// coercions
//

define method as (df-class == <double-float>, i :: <small-integer>)
  %int-to-double(i);
end method as;

define method as (i-class == <small-integer>, df :: <double-float>)
  %double-to-int(df);
end method as;

//
// <big-integer> coercions
//

// <small-integer> <-> <big-integer>

define method as (i-class == <small-integer>, bi :: <big-integer>)
  %bigint->int(bi);
end method as;

define method as (bi-class == <big-integer>, i :: <small-integer>)
  %int->bigint(i);
end method as;

// <double-float> <-> & <big-integer>

define method as (bi-class == <big-integer>, df :: <double-float>)
  %double->bigint(df);
end method as;

define method as (df-class == <double-float>, bi :: <big-integer>)
  %bigint->double(bi);
end method as;

// <character> <-> & <big-integer>

define method as (ch-class == <character>, bi :: <big-integer>)
  %integer->character(%bigint->int(bi));
end method as;

define method as (bi-class == <big-integer>, ch :: <character>)
  %int->bigint(%character->integer(ch));
end method as;

//
// Generic functions for arithmetic operations
//
define generic \+ (n1 :: <number>, n2 :: <number>) => value :: <number>;
define generic \* (n1 :: <number>, n2 :: <number>) => value :: <number>;
define generic \- (n1 :: <number>, n2 :: <number>) => value :: <number>;
define generic \/ (n1 :: <number>, n2 :: <number>) => value :: <number>;

define generic negative (n :: <number>) => value :: <number>;

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
// (op <small-integer> <small-integer>)
//
define method \+ (i1 :: <small-integer>, i2 :: <small-integer>)
  %binary-int+ (i1, i2);
end method \+;

define method \- (i1 :: <small-integer>, i2 :: <small-integer>)
  %binary-int- (i1, i2);
end method \-;

define method \* (i1 ::<small-integer>, i2 :: <small-integer>)
  %binary-int* (i1, i2);
end method \*;

define method negative (i :: <small-integer>) => value :: <small-integer>;
  %int-negative (i);
end method negative;

//
// (op <double-float> <double-float>)
//
define method \+ (d1 :: <double-float>, d2 :: <double-float>)
  %binary-double+ (d1, d2);
end method \+;

define method \- (d1 :: <double-float>, d2 :: <double-float>)
  %binary-double- (d1, d2);
end method \-;

define method \* (d1 :: <double-float>, d2 :: <double-float>)
  %binary-double* (d1, d2);
end method \*;

define method \/ (d1 :: <double-float>, d2 :: <double-float>)
  %binary-double/ (d1, d2);
end method \/;

define method negative (d :: <double-float>) => value :: <double-float>;
    %double-negative (d);
end method negative;

//
// (op <integer> <double-float>)
//
define method \+ (i1 :: <integer>, d2 :: <double-float>)
  %binary-double+ (as (<double-float>, i1), d2);
end method \+;

define method \- (i1 :: <integer>, d2 :: <double-float>)
  %binary-double- (as (<double-float>, i1), d2);
end method \-;

define method \* (i1 :: <integer>, d2 :: <double-float>)
  %binary-double* (as (<double-float>, i1), d2);
end method \*;

define method \/ (i1 :: <integer>, d2 :: <double-float>)
  %binary-double/ (as (<double-float>, i1), d2);
end method \/;

//
// (op <double-float> <integer>)
//
define method \+ (d1 :: <double-float>, i2 :: <integer>)
  %binary-double+ (d1, as (<double-float>, i2));
end method \+;

define method \- (d1 :: <double-float>, i2 :: <integer>)
  %binary-double- (d1, as (<double-float>, i2));
end method \-;

define method \* (d1 :: <double-float>, i2 :: <integer>)
  %binary-double* (d1, as (<double-float>, i2));
end method \*;

define method \/ (d1 :: <double-float>, i2 :: <integer>)
  %binary-double/ (d1, as (<double-float>, i2));
end method \/;

//
// (op <big-integer> <big-integer>) & friends.
//

define method \+ (b1 :: <big-integer>, b2 :: <big-integer>)
  %binary-bigint+ (b1, b2);
end method \+;

define method \+ (b :: <big-integer>, i :: <small-integer>)
  %binary-bigint+ (b, as(<big-integer>, i));
end method \+;

define method \+ (i :: <small-integer>, b :: <big-integer>)
  %binary-bigint+ (as(<big-integer>, i), b);
end method \+;

define method \- (b1 :: <big-integer>, b2 :: <big-integer>)
  %binary-bigint- (b1, b2);
end method \-;

define method \- (b :: <big-integer>, i :: <small-integer>)
  %binary-bigint- (b, as(<big-integer>, i));
end method \-;

define method \- (i :: <small-integer>, b :: <big-integer>)
  %binary-bigint- (as(<big-integer>, i), b);
end method \-;

define method \* (b1 ::<big-integer>, b2 :: <big-integer>)
  %binary-bigint* (b1, b2);
end method \*;

define method \* (b :: <big-integer>, i :: <small-integer>)
  %binary-bigint* (b, as(<big-integer>, i));
end method \*;

define method \* (i :: <small-integer>, b :: <big-integer>)
  %binary-bigint* (as(<big-integer>, i), b);
end method \*;

define method \/ (b :: <big-integer>, i :: <small-integer>)
   %binary-bigint/ (b, as(<big-integer>, i));
end method \/;

define method negative (b :: <big-integer>) => <big-integer>;
  %bigint-negative (b);
end method negative;

define method \^ (b :: <big-integer>, e :: <big-integer>) => <big-integer>;
      %bigint-pow(b, e);
end method \^;

define method \^ (b :: <big-integer>, e :: <small-integer>) => <big-integer>;
      %bigint-pow(b, as(<big-integer>, e));
end method \^;

define method \^ (b :: <small-integer>, e :: <big-integer>) => <big-integer>;
      %bigint-pow(as(<big-integer>, b), e);
end method \^;

/* Leave this out if <big-integer> is not implemented

define method \^ (b :: <small-integer>, e :: <small-integer>) => <big-integer>;
      %bigint-pow(as(<big-integer>, b), as(<big-integer>, e));
end method \^;

*/

//
// comparisons
//

define method \= (i :: <integer>, d :: <double-float>)
  as(<double-float>, i) == d;
end method \=;

define method \= (d :: <double-float>, i :: <integer>)
  d == as(<double-float>, i);
end method \=;

define method \< (n1 :: <number>, n2 :: <number>)
  %binary-less-than(n1, n2);
end method \<;

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

// between <big-integer>'s and <small-integer>'s.

define method \= (b1 :: <big-integer>, b2 :: <big-integer>)
  %bigint=(b1, b2);
end method \=;

define method \= (b :: <big-integer>, i :: <small-integer>)
  b = as(<big-integer>, i);
end method \=;

define method \= (i :: <small-integer>, b :: <big-integer>)
  b = as(<big-integer>, i);
end method \=;

define method \< (b1 :: <big-integer>, b2 :: <big-integer>)
  %bigint<(b1, b2);
end method \<;

define method \< (b :: <big-integer>, i :: <small-integer>)
  %bigint<(b, as(<big-integer>, i));
end method \<;

define method \< (i :: <small-integer>, b :: <big-integer>)
  %bigint<(as(<big-integer>, i), b);
end method \<;

//
// other functions
//

define generic abs (n :: <number>) => value :: <number>;

define method abs (i :: <small-integer>)
  %int-abs(i);
end method abs;

define method abs (bi :: <big-integer>)
  %bigint-abs(bi);
end method abs;

define method abs (d :: <double-float>)
  %double-abs(d);
end method abs;

define method ash (i :: <small-integer>, count :: <small-integer>)
  %ash(i, count);
end method ash;

define method logior (#rest integers)
  reduce1 (%binary-logior, integers);
end method logior;

define method logand (#rest integers)
  reduce1 (%binary-logand, integers);
end method logand;

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

define method floor (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %floor (d);
end method floor;

define method ceiling (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %ceiling (d);
end method ceiling;

define method round (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %round (d);
end method round;

define method truncate (d :: <double-float>)
 => (integer-part :: <integer>, remainder :: <double-float>);
  %truncate (d);
end method truncate;

define method modulo (n1 :: <real>, n2 :: <real>)
 => modulus :: <real>;
  %modulo-double ( as (<double-float>, n1), as (<double-float>, n2));
end method modulo;

define method modulo (i1 :: <small-integer>, i2 :: <small-integer>)
 => modulus :: <small-integer>;
  %modulo (i1, i2);
end method modulo;

define method remainder (n1 :: <real>, n2 :: <real>)
 => remainder :: <real>;
  %remainder-double (as (<double-float>, n1), as (<double-float>, n2));
end method remainder;

define method remainder (i1 :: <integer>, i2 :: <integer>)
 => remainder :: <integer>;
  %remainder-int (i1, i2);
end method remainder;

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

// <pcb> add <big-integer> cases again.
define method modulo (b1 :: <big-integer>, b2 :: <big-integer>)
  %bigint-mod (b1, b2);
end method modulo;

define method modulo (b :: <big-integer>, i :: <small-integer>)
      %bigint-mod (b, as (<big-integer>, i));
end method modulo;

define method modulo (i :: <small-integer>, g :: <big-integer>)
      %bigint-mod (as (<big-integer>, i), b);
end method modulo;

define method floor/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %floor/ (as (<double-float>, n1), as (<double-float>, n2));
end method floor/;

define method ceiling/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %ceiling/ (as (<double-float>, n1), as (<double-float>, n2));
end method ceiling/;

define method round/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %round/ (as (<double-float>, n1), as (<double-float>, n2));
end method round/;

define method truncate/ (n1 :: <real>, n2 :: <real>)
 => (quotient :: <integer>, remainder :: <double-float>);
  %truncate/ (as (<double-float>, n1), as (<double-float>, n2));
end method truncate/;

define method truncate/ (i1 :: <small-integer>, i2 :: <small-integer>)
 => (quotient :: <small-integer>, remainder :: <small-integer>);
  %int-truncate/ (i1, i2);
end method truncate/;

//
// Non-DIRM arithmetic operations
//

define method sqrt (i :: <small-integer>)
  %int-sqrt(i);
end method sqrt;

define method sqrt (bi :: <big-integer>)
  %bigint-sqrt(bi);
end method sqrt;

define method sqrt (d :: <double-float>)
  %double-sqrt(d);
end method sqrt;

define method sin (n :: <number>) => value :: <number>;
  %sin (as (<double-float>, n));
end method sin;

define method cos (n :: <number>) => value :: <number>;
  %cos (as (<double-float>, n));
end method cos;

define method atan2 (d1 :: <number>, d2 :: <number>)
  %atan2 (as (<double-float>, d1), as (<double-float>, d2));
end method atan2;

define method exp (n :: <number>)
  %exp(as(<double-float>, n));
end method exp;

define method ln (n :: <number>)
  %ln(as(<double-float>, n));
end method ln;

// end number.dyl
