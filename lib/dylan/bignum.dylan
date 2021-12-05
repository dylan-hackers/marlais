module: dylan

//
// Number coercion
//

// From other numbers and our own types.
//
// Our primitives support a range of types.

define method as (c == <big-float>, n :: <number>)
  %number->mpf (n);
end method as;

define method as (c == <big-ratio>, n :: <number>)
  %number->mpq (n);
end method as;

define method as (c == <big-integer>, n :: <number>)
  %number->mpz (n);
end method as;

// From strings.
//
// We expect base 10. Use simple string coercion.

define method as (c == <big-float>, s :: <string>)
  %string->mpf (as (<byte-string>, s), 10);
end method as;

define method as (c == <big-ratio>, s :: <string>)
  %string->mpq (as (<byte-string>, s), 10);
end method as;

define method as (c == <big-integer>, s :: <string>)
  %string->mpz (as (<byte-string>, s), 10);
end method as;

//
// Numeric predicates
//

// <big-float>

define method zero? (n :: <big-float>) => value :: <boolean>;
  %mpf-zero?(n);
end method;

define method positive? (n :: <big-float>) => value :: <boolean>;
  %mpf-positive?(n);
end method;

define method negative? (n :: <big-float>) => value :: <boolean>;
  %mpf-negative?(n);
end method;

// <big-ratio>

define method zero? (n :: <big-ratio>) => value :: <boolean>;
  %mpq-zero?(n);
end method;

define method positive? (n :: <big-ratio>) => value :: <boolean>;
  %mpq-positive?(n);
end method;

define method negative? (n :: <big-ratio>) => value :: <boolean>;
  %mpq-negative?(n);
end method;

// <big-integer>

define method odd? (n :: <big-integer>) => value :: <boolean>;
  %mpz-odd?(n);
end method;

define method even? (n :: <big-integer>) => value :: <boolean>;
  %mpz-even?(n);
end method;

define method zero? (n :: <big-integer>) => value :: <boolean>;
  %mpz-zero?(n);
end method;

define method positive? (n :: <big-integer>) => value :: <boolean>;
  %mpz-positive?(n);
end method;

define method negative? (n :: <big-integer>) => value :: <boolean>;
  %mpz-negative?(n);
end method;

//
// Number comparison
//

// Among our own types

define method \< (n1 :: <big-float>, n2 :: <big-float>)
  %mpf< (n1, n2);
end method \<;

define method \= (n1 :: <big-float>, n2 :: <big-float>)
  %mpf= (n1, n2);
end method \=;

define method \< (n1 :: <big-ratio>, n2 :: <big-ratio>)
  %mpq< (n1, n2);
end method \<;

define method \= (n1 :: <big-ratio>, n2 :: <big-ratio>)
  %mpq= (n1, n2);
end method \=;

define method \< (n1 :: <big-integer>, n2 :: <big-integer>)
  %mpz< (n1, n2);
end method \<;

define method \= (n1 :: <big-integer>, n2 :: <big-integer>)
  %mpz= (n1, n2);
end method \=;

//
// Binary operations
//

// \+ on <big-float>

define method \+ (i1 :: <big-float>, i2 :: <big-float>)
  %mpf-add (i1, i2);
end method \+;

define method \+ (i1 :: <big-float>, o2 :: <number>)
  %mpf-add (i1, o2);
end method \+;

define method \+ (o1 :: <number>, i2 :: <big-float>)
  %mpf-add (i2, o1);
end method \+;

// \+ on <big-integer>

define method \+ (i1 :: <big-integer>, i2 :: <big-integer>)
  %mpz-add (i1, i2);
end method \+;

define method \+ (i1 :: <big-integer>, o2 :: <integer>)
  %mpz-add (i1, o2);
end method \+;

define method \+ (o1 :: <integer>, i2 :: <big-integer>)
  %mpz-add (i2, o1);
end method \+;

// \+ on <big-ratio>

define method \+ (i1 :: <big-ratio>, i2 :: <big-ratio>)
  %mpq-add (i1, i2);
end method \+;

// \- on <big-float>

define method \- (i1 :: <big-float>, i2 :: <big-float>)
  %mpf-sub (i1, i2);
end method \-;

define method \- (i1 :: <big-float>, o2 :: <number>)
  %mpf-sub (i1, o2);
end method \-;

define method \- (o1 :: <number>, i2 :: <big-float>)
  %mpf-sub (o1, i2);
end method \-;

// \- on <big-integer>

define method \- (i1 :: <big-integer>, i2 :: <big-integer>)
  %mpz-sub (i1, i2);
end method \-;

define method \- (i1 :: <big-integer>, o2 :: <integer>)
  %mpz-sub (i1, o2);
end method \-;

define method \- (o1 :: <integer>, i2 :: <big-integer>)
  %mpz-sub (o1, i2);
end method \-;

// \- on <big-ratio>
define method \- (i1 :: <big-ratio>, i2 :: <big-ratio>)
  %mpq-sub (i1, i2);
end method \-;

// \* on <big-float>

define method \* (i1 :: <big-float>, i2 :: <big-float>)
  %mpf-mul (i1, i2);
end method \*;

define method \* (i1 :: <big-float>, o2 :: <number>)
  %mpf-mul (i1, o2);
end method \*;

define method \* (o1 :: <number>, i2 :: <big-float>)
  %mpf-mul (i2, o1);
end method \*;

// \* on <big-float>

define method \* (i1 :: <big-integer>, i2 :: <big-integer>)
  %mpz-mul (i1, i2);
end method \*;

define method \* (i1 :: <big-integer>, o2 :: <integer>)
  %mpz-mul (i1, o2);
end method \*;

define method \* (o1 :: <integer>, i2 :: <big-integer>)
  %mpz-mul (i2, o1);
end method \*;

// \* on <big-ratio>

define method \* (i1 :: <big-ratio>, i2 :: <big-ratio>)
  %mpq-mul (i1, i2);
end method \*;

//
// Unary operations
//

// On <big-float>

define method negative (n :: <big-float>)
  %mpf-neg(n);
end method negative;

define method abs (n :: <big-float>)
  %mpf-abs(n);
end method abs;

define method sqrt (n :: <big-float>)
  %mpf-sqrt(n);
end method sqrt;

define method ceiling (n :: <big-float>)
  // XXX no remainder
  %mpf-ceil(n);
end method ceiling;

define method floor (n :: <big-float>)
  // XXX no remainder
  %mpf-floor(n);
end method floor;

define method truncate (n :: <big-float>)
  // XXX no remainder
  %mpf-trunc(n);
end method truncate;

// On <big-ratio>

define method negative (n :: <big-ratio>)
  %mpq-neg(n);
end method neg;

define method abs (n :: <big-ratio>)
  %mpq-abs(n);
end method abs;

// On <big-integer>

define method negative (n :: <big-integer>)
  %mpz-neg(n);
end method neg;

define method abs (n :: <big-integer>)
  %mpz-abs(n);
end method abs;
