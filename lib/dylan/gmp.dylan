module: dylan

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

define method \= (n1 :: <mp-float>, n2 :: <mp-float>)
  %mpf=(n1, n2);
end method \=;

define method \< (n1 :: <mp-float>, n2 :: <mp-float>)
  %mpf<(n1, n2);
end method \<;

define method \= (n1 :: <mp-ratio>, n2 :: <mp-ratio>)
  %mpq=(n1, n2);
end method \=;

define method \< (n1 :: <mp-ratio>, n2 :: <mp-ratio>)
  %mpq<(n1, n2);
end method \<;

define method \= (n1 :: <mp-integer>, n2 :: <mp-integer>)
  %mpz=(n1, n2);
end method \=;

define method \< (n1 :: <mp-integer>, n2 :: <mp-integer>)
  %mpz<(n1, n2);
end method \<;

//
// Number coercion
//

// From other numbers and our own types.
//
// Our primitives support a range of types.

define method as (c == <mp-float>, n :: <number>)
  %number->mpf(n);
end method as;

define method as (c == <mp-ratio>, n :: <number>)
  %number->mpq(n);
end method as;

define method as (c == <mp-integer>, n :: <number>)
  %number->mpz(n);
end method as;

// From strings.
//
// We expect base 10. Only byte string for now.

define method as (c == <mp-float>, s :: <byte-string>)
  %string->mpf(n, 10);
end method as;

define method as (c == <mp-ratio>, s :: <byte-string>)
  %string->mpq(n, 10);
end method as;

define method as (c == <mp-integer>, s :: <byte-string>)
  %string->mpz(n, 10);
end method as;
