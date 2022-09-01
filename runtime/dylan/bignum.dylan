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

// Each of our own types to itself
//
// These call the primitives directly.

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

// <big-float> with any real
//
// Coerce the other real to <big-float>.

define method \< (n1 :: <big-float>, n2 :: <real>)
  %mpf< (n1, as(<big-float>, n2));
end method \<;

define method \< (n1 :: <real>, n2 :: <big-float>)
  %mpf< (as(<big-float>, n1), n2);
end method \<;

define method \= (n1 :: <big-float>, n2 :: <real>)
  %mpf= (n1, as(<big-float>, n2));
end method \=;

define method \= (n1 :: <real>, n2 :: <big-float>)
  %mpf= (as(<big-float>, n1), n2);
end method \<;

// <big-float> with any rational
//
// Coerce the rational to <big-float>.

define method \< (n1 :: <big-float>, n2 :: <rational>)
  %mpf< (n1, as(<big-float>, n2));
end method \<;

define method \< (n1 :: <rational>, n2 :: <big-float>)
  %mpf< (as(<big-float>, n1), n2);
end method \<;

define method \= (n1 :: <big-float>, n2 :: <rational>)
  %mpf= (n1, as(<big-float>, n2));
end method \=;

define method \= (n1 :: <rational>, n2 :: <big-float>)
  %mpf= (as(<big-float>, n1), n2);
end method \<;

// <big-float> with any integer
//
// Coerce the integer to <big-float>.

define method \< (n1 :: <big-float>, n2 :: <integer>)
  %mpf< (n1, as(<big-float>, n2));
end method \<;

define method \< (n1 :: <integer>, n2 :: <big-float>)
  %mpf< (as(<big-float>, n1), n2);
end method \<;

define method \= (n1 :: <big-float>, n2 :: <integer>)
  %mpf= (n1, as(<big-float>, n2));
end method \=;

define method \= (n1 :: <integer>, n2 :: <big-float>)
  %mpf= (as(<big-float>, n1), n2);
end method \<;

// <big-ratio> with any real
//
// Coerce both to <big-float>.

define method \< (n1 :: <big-ratio>, n2 :: <real>)
  %mpf< (as(<big-float>, n1), as(<big-float>, n2));
end method \<;

define method \< (n1 :: <real>, n2 :: <big-ratio>)
  %mpf< (as(<big-float>, n1), as(<big-float>, n2));
end method \<;

define method \= (n1 :: <big-ratio>, n2 :: <real>)
  %mpf= (as(<big-float>, n1), as(<big-float>, n2));
end method \=;

define method \= (n1 :: <real>, n2 :: <big-ratio>)
  %mpf= (as(<big-float>, n1), as(<big-float>, n2));
end method \<;

// <big-ratio> with any rational
//
// Coerce the other rational to <big-ratio>.

define method \< (n1 :: <big-ratio>, n2 :: <rational>)
  %mpq< (n1, as(<big-ratio>, n2));
end method \<;

define method \< (n1 :: <rational>, n2 :: <big-ratio>)
  %mpq< (as(<big-ratio>, n1), n2);
end method \<;

define method \= (n1 :: <big-ratio>, n2 :: <rational>)
  %mpq= (n1, as(<big-ratio>, n2));
end method \=;

define method \= (n1 :: <rational>, n2 :: <big-ratio>)
  %mpq= (as(<big-ratio>, n1), n2);
end method \<;

// <big-ratio> with any integer
//
// Coerce the integer to <big-ratio>.

define method \< (n1 :: <big-ratio>, n2 :: <integer>)
  %mpq< (n1, as(<big-ratio>, n2));
end method \<;

define method \< (n1 :: <integer>, n2 :: <big-ratio>)
  %mpq< (as(<big-ratio>, n1), n2);
end method \<;

define method \= (n1 :: <big-ratio>, n2 :: <integer>)
  %mpq= (n1, as(<big-ratio>, n2));
end method \=;

define method \= (n1 :: <integer>, n2 :: <big-ratio>)
  %mpq= (as(<big-ratio>, n1), n2);
end method \<;

// <big-integer> with any real
//
// Coerce both to <big-float>.

define method \< (n1 :: <big-integer>, n2 :: <real>)
  %mpf< (as(<big-float>, n1), as(<big-float>, n2));
end method \<;

define method \< (n1 :: <real>, n2 :: <big-integer>)
  %mpf< (as(<big-float>, n1), as(<big-float>, n2));
end method \<;

define method \= (n1 :: <big-integer>, n2 :: <real>)
  %mpf= (as(<big-float>, n1), as(<big-float>, n2));
end method \=;

define method \= (n1 :: <real>, n2 :: <big-integer>)
  %mpf= (as(<big-float>, n1), as(<big-float>, n2));
end method \<;

// <big-integer> with any rational
//
// Coerce both to <big-ratio>.

define method \< (n1 :: <big-integer>, n2 :: <rational>)
  %mpq< (as(<big-ratio>, n1), as(<big-ratio>, n2));
end method \<;

define method \< (n1 :: <rational>, n2 :: <big-integer>)
  %mpq< (as(<big-ratio>, n1), as(<big-ratio>, n2));
end method \<;

define method \= (n1 :: <big-integer>, n2 :: <rational>)
  %mpq= (as(<big-ratio>, n1), as(<big-ratio>, n2));
end method \=;

define method \= (n1 :: <rational>, n2 :: <big-integer>)
  %mpq= (as(<big-ratio>, n1), as(<big-ratio>, n2));
end method \<;

// <big-integer> with any integer
//
// Coerce the other integer to <big-integer>.

define method \< (n1 :: <big-integer>, n2 :: <integer>)
  %mpz< (n1, as(<big-integer>, n2));
end method \<;

define method \< (n1 :: <integer>, n2 :: <big-integer>)
  %mpz< (as(<big-integer>, n1), n2);
end method \<;

define method \= (n1 :: <big-integer>, n2 :: <integer>)
  %mpz= (n1, as(<big-integer>, n2));
end method \=;

define method \= (n1 :: <integer>, n2 :: <big-integer>)
  %mpz= (as(<big-integer>, n1), n2);
end method \<;

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

//
// Binary operations
//

// <big-float>
//
// Simple cases.

define method \+ (i1 :: <big-float>, i2 :: <big-float>)
  %mpf-add (i1, i2);
end method \+;

define method \- (i1 :: <big-float>, i2 :: <big-float>)
  %mpf-sub (i1, i2);
end method \-;

define method \* (i1 :: <big-float>, i2 :: <big-float>)
  %mpf-mul (i1, i2);
end method \*;

// <big-ratio>
//
// Simple cases.

define method \+ (i1 :: <big-ratio>, i2 :: <big-ratio>)
  %mpq-add (i1, i2);
end method \+;

define method \- (i1 :: <big-ratio>, i2 :: <big-ratio>)
  %mpq-sub (i1, i2);
end method \-;

define method \* (i1 :: <big-ratio>, i2 :: <big-ratio>)
  %mpq-mul (i1, i2);
end method \*;

// <big-integer>
//
// Simple cases.

define method \+ (i1 :: <big-integer>, i2 :: <big-integer>)
  %mpz-add (i1, i2);
end method \+;

define method \- (i1 :: <big-integer>, i2 :: <big-integer>)
  %mpz-sub (i1, i2);
end method \-;

define method \* (i1 :: <big-integer>, i2 :: <big-integer>)
  %mpz-mul (i1, i2);
end method \*;

// <big-integer> and small integer
//
// Native cases.

define method \+ (i1 :: <big-integer>, o2 :: <small-integer>)
  %mpz-add (i1, o2);
end method \+;

define method \+ (o1 :: <small-integer>, i2 :: <big-integer>)
  %mpz-add (o1, i2);
end method \+;

define method \- (i1 :: <big-integer>, o2 :: <small-integer>)
  %mpz-sub (i1, o2);
end method \-;

define method \- (o1 :: <small-integer>, i2 :: <big-integer>)
  %mpz-sub (o1, i2);
end method \-;

define method \* (i1 :: <big-integer>, o2 :: <small-integer>)
  %mpz-mul (i1, o2);
end method \*;

define method \* (o1 :: <small-integer>, i2 :: <big-integer>)
  %mpz-mul (o1, i2);
end method \*;

// <big-float> and any real
//
// Coerce the other real to <big-float>.

define method \+ (n1 :: <big-float>, n2 :: <real>)
  %mpf-add (n1, as (<big-float>, n2));
end method \+;

define method \+ (n1 :: <real>, n2 :: <big-float>)
  %mpf-add (as (<big-float>, n1), n2);
end method \+;

define method \- (n1 :: <big-float>, n2 :: <real>)
  %mpf-sub (n1, as (<big-float>, n2));
end method \-;

define method \- (n1 :: <real>, n2 :: <big-float>)
  %mpf-sub (as (<big-float>, n1), n2);
end method \-;

define method \* (n1 :: <big-float>, n2 :: <real>)
  %mpf-mul (n1, as (<big-float>, n2));
end method \*;

define method \* (n1 :: <real>, n2 :: <big-float>)
  %mpf-mul (as (<big-float>, n1), n2);
end method \*;

// <big-float> and any rational
//
// Coerce the rational to <big-float>.

define method \+ (n1 :: <big-float>, n2 :: <rational>)
  %mpf-add (n1, as (<big-float>, n2));
end method \+;

define method \+ (n1 :: <rational>, n2 :: <big-float>)
  %mpf-add (as (<big-float>, n1), n2);
end method \+;

define method \- (n1 :: <big-float>, n2 :: <rational>)
  %mpf-sub (n1, as (<big-float>, n2));
end method \-;

define method \- (n1 :: <rational>, n2 :: <big-float>)
  %mpf-sub (as (<big-float>, n1), n2);
end method \-;

define method \* (n1 :: <big-float>, n2 :: <rational>)
  %mpf-mul (n1, as (<big-float>, n2));
end method \*;

define method \* (n1 :: <rational>, n2 :: <big-float>)
  %mpf-mul (as (<big-float>, n1), n2);
end method \*;

// <big-float> and any integer
//
// Coerce the integer to <big-float>.

define method \+ (n1 :: <big-float>, n2 :: <integer>)
  %mpf-add (n1, as (<big-float>, n2));
end method \+;

define method \+ (n1 :: <integer>, n2 :: <big-float>)
  %mpf-add (as (<big-float>, n1), n2);
end method \+;

define method \- (n1 :: <big-float>, n2 :: <integer>)
  %mpf-sub (n1, as (<big-float>, n2));
end method \-;

define method \- (n1 :: <integer>, n2 :: <big-float>)
  %mpf-sub (as (<big-float>, n1), n2);
end method \-;

define method \* (n1 :: <big-float>, n2 :: <integer>)
  %mpf-mul (n1, as (<big-float>, n2));
end method \*;

define method \* (n1 :: <integer>, n2 :: <big-float>)
  %mpf-mul (as (<big-float>, n1), n2);
end method \*;

// <big-ratio> and any real
//
// Coerce both to <big-float>

define method \+ (n1 :: <big-ratio>, n2 :: <real>)
  %mpf-add (as (<big-float>, n1), as (<big-float>, n2));
end method \+;

define method \+ (n1 :: <real>, n2 :: <big-ratio>)
  %mpf-add (as (<big-float>, n1), as (<big-float>, n2));
end method \+;

define method \- (n1 :: <big-ratio>, n2 :: <real>)
  %mpf-sub (as (<big-float>, n1), as (<big-float>, n2));
end method \-;

define method \- (n1 :: <real>, n2 :: <big-ratio>)
  %mpf-sub (as (<big-float>, n1), as (<big-float>, n2));
end method \-;

define method \* (n1 :: <big-ratio>, n2 :: <real>)
  %mpf-mul (as (<big-float>, n1), as (<big-float>, n2));
end method \*;

define method \* (n1 :: <real>, n2 :: <big-ratio>)
  %mpf-mul (as (<big-float>, n1), as (<big-float>));
end method \*;

// <big-ratio> and any rational
//
// Coerce the other rational to <big-ratio>.

define method \+ (n1 :: <big-ratio>, n2 :: <rational>)
  %mpq-add (n1, as (<big-ratio>, n2));
end method \+;

define method \+ (n1 :: <rational>, n2 :: <big-ratio>)
  %mpq-add (as (<big-ratio>, n1), n2);
end method \+;

define method \- (n1 :: <big-ratio>, n2 :: <rational>)
  %mpq-sub (n1, as (<big-ratio>, n2));
end method \-;

define method \- (n1 :: <rational>, n2 :: <big-ratio>)
  %mpq-sub (as (<big-ratio>, n1), n2);
end method \-;

define method \* (n1 :: <big-ratio>, n2 :: <rational>)
  %mpq-mul (n1, as (<big-ratio>, n2));
end method \*;

define method \* (n1 :: <rational>, n2 :: <big-ratio>)
  %mpq-mul (as (<big-ratio>, n1), n2);
end method \*;

// <big-ratio> and any integer
//
// Coerce the integer to <big-ratio>.

define method \+ (n1 :: <big-ratio>, n2 :: <integer>)
  %mpq-add (n1, as (<big-ratio>, n2));
end method \+;

define method \+ (n1 :: <integer>, n2 :: <big-ratio>)
  %mpq-add (as (<big-ratio>, n1), n2);
end method \+;

define method \- (n1 :: <big-ratio>, n2 :: <integer>)
  %mpq-sub (n1, as (<big-ratio>, n2));
end method \-;

define method \- (n1 :: <integer>, n2 :: <big-ratio>)
  %mpq-sub (as (<big-ratio>, n1), n2);
end method \-;

define method \* (n1 :: <big-ratio>, n2 :: <integer>)
  %mpq-mul (n1, as (<big-ratio>, n2));
end method \*;

define method \* (n1 :: <integer>, n2 :: <big-ratio>)
  %mpq-mul (as (<big-ratio>, n1), n2);
end method \*;

// <big-integer> and any real
//
// Coerce both to <big-float>

define method \+ (n1 :: <big-integer>, n2 :: <real>)
  %mpf-add (as (<big-float>, n1), as (<big-float>, n2));
end method \+;

define method \+ (n1 :: <real>, n2 :: <big-integer>)
  %mpf-add (as (<big-float>, n1), as (<big-float>, n2));
end method \+;

define method \- (n1 :: <big-integer>, n2 :: <real>)
  %mpf-sub (as (<big-float>, n1), as (<big-float>, n2));
end method \-;

define method \- (n1 :: <real>, n2 :: <big-integer>)
  %mpf-sub (as (<big-float>, n1), as (<big-float>, n2));
end method \-;

define method \* (n1 :: <big-integer>, n2 :: <real>)
  %mpf-mul (as (<big-float>, n1), as (<big-float>, n2));
end method \*;

define method \* (n1 :: <real>, n2 :: <big-integer>)
  %mpf-mul (as (<big-float>, n1), as (<big-float>));
end method \*;

// <big-integer> and any rational
//
// Coerce both to <big-ratio>

define method \+ (n1 :: <big-integer>, n2 :: <rational>)
  %mpq-add (as (<big-ratio>, n1), as (<big-ratio>, n2));
end method \+;

define method \+ (n1 :: <rational>, n2 :: <big-integer>)
  %mpq-add (as (<big-ratio>, n1), as (<big-ratio>, n2));
end method \+;

define method \- (n1 :: <big-integer>, n2 :: <rational>)
  %mpq-sub (as (<big-ratio>, n1), as (<big-ratio>, n2));
end method \-;

define method \- (n1 :: <rational>, n2 :: <big-integer>)
  %mpq-sub (as (<big-ratio>, n1), as (<big-ratio>, n2));
end method \-;

define method \* (n1 :: <big-integer>, n2 :: <rational>)
  %mpq-mul (as (<big-ratio>, n1), as (<big-ratio>, n2));
end method \*;

define method \* (n1 :: <rational>, n2 :: <big-integer>)
  %mpq-mul (as (<big-ratio>, n1), as (<big-ratio>), n2);
end method \*;

// <big-integer> and any integer
//
// Coerce the other integer to <big-integer>.

define method \+ (n1 :: <big-integer>, n2 :: <integer>)
  %mpz-add (n1, as (<big-integer>, n2));
end method \+;

define method \+ (n1 :: <integer>, n2 :: <big-integer>)
  %mpz-add (as (<big-integer>, n1), n2);
end method \+;

define method \- (n1 :: <big-integer>, n2 :: <integer>)
  %mpz-sub (n1, as (<big-integer>, n2));
end method \-;

define method \- (n1 :: <integer>, n2 :: <big-integer>)
  %mpz-sub (as (<big-integer>, n1), n2);
end method \-;

define method \* (n1 :: <big-integer>, n2 :: <integer>)
  %mpz-mul (n1, as (<big-integer>, n2));
end method \*;

define method \* (n1 :: <integer>, n2 :: <big-integer>)
  %mpz-mul (as (<big-integer>, n1), n2);
end method \*;
