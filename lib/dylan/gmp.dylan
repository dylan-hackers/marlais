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
// We expect base 10. Only byte string for now.

define method as (c == <mp-float>, s :: <byte-string>)
  %string->mpf (s, 10);
end method as;

define method as (c == <mp-ratio>, s :: <byte-string>)
  %string->mpq (s, 10);
end method as;

define method as (c == <mp-integer>, s :: <byte-string>)
  %string->mpz (s, 10);
end method as;

//
// Numeric predicates
//

// <mp-float>

define method zero? (n :: <mp-float>) => value :: <boolean>;
  %mpf-zero?(mpz);
end method;

define method positive? (n :: <mp-float>) => value :: <boolean>;
  %mpf-positive?(mpz);
end method;

define method negative? (n :: <mp-float>) => value :: <boolean>;
  %mpf-negative?(mpz);
end method;

// <mp-ratio>

define method zero? (n :: <mp-ratio>) => value :: <boolean>;
  %mpq-zero?(mpz);
end method;

define method positive? (n :: <mp-ratio>) => value :: <boolean>;
  %mpq-positive?(mpz);
end method;

define method negative? (n :: <mp-ratio>) => value :: <boolean>;
  %mpq-negative?(mpz);
end method;

// <mp-integer>

define method odd? (n :: <mp-integer>) => value :: <boolean>;
  %mpz-odd?(mpz);
end method;

define method even? (n :: <mp-integer>) => value :: <boolean>;
  %mpz-even?(mpz);
end method;

define method zero? (n :: <mp-integer>) => value :: <boolean>;
  %mpz-zero?(mpz);
end method;

define method positive? (n :: <mp-integer>) => value :: <boolean>;
  %mpz-positive?(mpz);
end method;

define method negative? (n :: <mp-integer>) => value :: <boolean>;
  %mpz-negative?(mpz);
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

// \+

// \-

// \*

//
// Unary operations
//

// On <mp-float>

define method negative (n :: <mp-float>)
  %mpf-neg(n);
end method negative;

define method abs (n :: <mp-float>)
  %mpf-sqrt(n);
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
