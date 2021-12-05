module: dylan

//
// Number coercion
//

// From other numbers and our own types.
//
// Our primitives support a range of types.

define method as (c == <mp-float>, n :: <number>)
  %number->mpf (n);
end method as;

define method as (c == <mp-ratio>, n :: <number>)
  %number->mpq (n);
end method as;

define method as (c == <mp-integer>, n :: <number>)
  %number->mpz (n);
end method as;

// From strings.
//
// We expect base 10. Use simple string coercion.

define method as (c == <mp-float>, s :: <string>)
  %string->mpf (as (<byte-string>, s), 10);
end method as;

define method as (c == <mp-ratio>, s :: <string>)
  %string->mpq (as (<byte-string>, s), 10);
end method as;

define method as (c == <mp-integer>, s :: <string>)
  %string->mpz (as (<byte-string>, s), 10);
end method as;

//
// Numeric predicates
//

// <mp-float>

define method zero? (n :: <mp-float>) => value :: <boolean>;
  %mpf-zero?(n);
end method;

define method positive? (n :: <mp-float>) => value :: <boolean>;
  %mpf-positive?(n);
end method;

define method negative? (n :: <mp-float>) => value :: <boolean>;
  %mpf-negative?(n);
end method;

// <mp-ratio>

define method zero? (n :: <mp-ratio>) => value :: <boolean>;
  %mpq-zero?(n);
end method;

define method positive? (n :: <mp-ratio>) => value :: <boolean>;
  %mpq-positive?(n);
end method;

define method negative? (n :: <mp-ratio>) => value :: <boolean>;
  %mpq-negative?(n);
end method;

// <mp-integer>

define method odd? (n :: <mp-integer>) => value :: <boolean>;
  %mpz-odd?(n);
end method;

define method even? (n :: <mp-integer>) => value :: <boolean>;
  %mpz-even?(n);
end method;

define method zero? (n :: <mp-integer>) => value :: <boolean>;
  %mpz-zero?(n);
end method;

define method positive? (n :: <mp-integer>) => value :: <boolean>;
  %mpz-positive?(n);
end method;

define method negative? (n :: <mp-integer>) => value :: <boolean>;
  %mpz-negative?(n);
end method;

//
// Number comparison
//

// Among our own types

define method \< (n1 :: <mp-float>, n2 :: <mp-float>)
  %mpf< (n1, n2);
end method \<;

define method \= (n1 :: <mp-float>, n2 :: <mp-float>)
  %mpf= (n1, n2);
end method \=;

define method \< (n1 :: <mp-ratio>, n2 :: <mp-ratio>)
  %mpq< (n1, n2);
end method \<;

define method \= (n1 :: <mp-ratio>, n2 :: <mp-ratio>)
  %mpq= (n1, n2);
end method \=;

define method \< (n1 :: <mp-integer>, n2 :: <mp-integer>)
  %mpz< (n1, n2);
end method \<;

define method \= (n1 :: <mp-integer>, n2 :: <mp-integer>)
  %mpz= (n1, n2);
end method \=;

//
// Binary operations
//

// \+ on <mp-float>

define method \+ (i1 :: <mp-float>, i2 :: <mp-float>)
  %mpf-add (i1, i2);
end method \+;

define method \+ (i1 :: <mp-float>, o2 :: <number>)
  %mpf-add (i1, o2);
end method \+;

define method \+ (o1 :: <number>, i2 :: <mp-float>)
  %mpf-add (i2, o1);
end method \+;

// \+ on <mp-integer>

define method \+ (i1 :: <mp-integer>, i2 :: <mp-integer>)
  %mpz-add (i1, i2);
end method \+;

define method \+ (i1 :: <mp-integer>, o2 :: <integer>)
  %mpz-add (i1, o2);
end method \+;

define method \+ (o1 :: <integer>, i2 :: <mp-integer>)
  %mpz-add (i2, o1);
end method \+;

// \+ on <mp-ratio>

define method \+ (i1 :: <mp-ratio>, i2 :: <mp-ratio>)
  %mpq-add (i1, i2);
end method \+;

// \- on <mp-float>

define method \- (i1 :: <mp-float>, i2 :: <mp-float>)
  %mpf-sub (i1, i2);
end method \-;

define method \- (i1 :: <mp-float>, o2 :: <number>)
  %mpf-sub (i1, o2);
end method \-;

define method \- (o1 :: <number>, i2 :: <mp-float>)
  %mpf-sub (o1, i2);
end method \-;

// \- on <mp-integer>

define method \- (i1 :: <mp-integer>, i2 :: <mp-integer>)
  %mpz-sub (i1, i2);
end method \-;

define method \- (i1 :: <mp-integer>, o2 :: <integer>)
  %mpz-sub (i1, o2);
end method \-;

define method \- (o1 :: <integer>, i2 :: <mp-integer>)
  %mpz-sub (o1, i2);
end method \-;

// \- on <mp-ratio>
define method \- (i1 :: <mp-ratio>, i2 :: <mp-ratio>)
  %mpq-sub (i1, i2);
end method \-;

// \* on <mp-float>

define method \* (i1 :: <mp-float>, i2 :: <mp-float>)
  %mpf-mul (i1, i2);
end method \*;

define method \* (i1 :: <mp-float>, o2 :: <number>)
  %mpf-mul (i1, o2);
end method \*;

define method \* (o1 :: <number>, i2 :: <mp-float>)
  %mpf-mul (i2, o1);
end method \*;

// \* on <mp-float>

define method \* (i1 :: <mp-integer>, i2 :: <mp-integer>)
  %mpz-mul (i1, i2);
end method \*;

define method \* (i1 :: <mp-integer>, o2 :: <integer>)
  %mpz-mul (i1, o2);
end method \*;

define method \* (o1 :: <integer>, i2 :: <mp-integer>)
  %mpz-mul (i2, o1);
end method \*;

// \* on <mp-ratio>

define method \* (i1 :: <mp-ratio>, i2 :: <mp-ratio>)
  %mpq-mul (i1, i2);
end method \*;

//
// Unary operations
//

// On <mp-float>

define method negative (n :: <mp-float>)
  %mpf-neg(n);
end method negative;

define method abs (n :: <mp-float>)
  %mpf-abs(n);
end method abs;

define method sqrt (n :: <mp-float>)
  %mpf-sqrt(n);
end method sqrt;

define method ceiling (n :: <mp-float>)
  // XXX no remainder
  %mpf-ceil(n);
end method ceiling;

define method floor (n :: <mp-float>)
  // XXX no remainder
  %mpf-floor(n);
end method floor;

define method truncate (n :: <mp-float>)
  // XXX no remainder
  %mpf-trunc(n);
end method truncate;

// On <mp-ratio>

define method negative (n :: <mp-ratio>)
  %mpq-neg(n);
end method neg;

define method abs (n :: <mp-ratio>)
  %mpq-abs(n);
end method abs;

// On <mp-integer>

define method negative (n :: <mp-integer>)
  %mpz-neg(n);
end method neg;

define method abs (n :: <mp-integer>)
  %mpz-abs(n);
end method abs;
