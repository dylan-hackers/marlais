module: dylan

// character.dylan
//
// Authors:
//   Brent Benson
//   Ingo Albrecht
//

// TODO  Could save memory by eliminating separate
//       primitives for each character type. They
//       could be combined using macro trickery
//       in character.c.

//
// Codepoint types
//

// We expect that codepoints always fit in small integers
define constant <codepoint> = <small-integer>;

// Byte codepoints are always the same
define constant <byte-codepoint> =
  limited (<integer>,
           min: $minimum-byte,
           max: $maximum-byte);

// Wide codepoints depend on the system
define constant <wide-codepoint> =
  limited (<integer>,
           min: $minimum-wide-codepoint,
           max: $maximum-wide-codepoint);

// Unicode codepoints depend on the specification
define constant <unicode-codepoint> =
  limited (<integer>,
           min: $minimum-unicode-codepoint,
           max: $maximum-unicode-codepoint);

//
// Character comparison
//

define method \< (c1 :: <byte-character>, c2 :: <byte-character>)
  %character->integer(c1) < %character->integer(c2);
end method \<;

define method \< (c1 :: <wide-character>, c2 :: <wide-character>)
  %wchar->integer(c1) < %wchar->integer(c2);
end method \<;

define method \< (c1 :: <unicode-character>, c2 :: <unicode-character>)
  %uchar->integer(c1) < %uchar->integer(c2);
end method \<;

//
// Character coercion
//

// Between characters and integers

define method as (cc == <character>, ch :: <small-integer>)
  as(<standard-character>, ch)
end method as;

define method as (ic == <integer>, ch :: <byte-character>)
  %character->integer(ch);
end method as;

define method as (cc == <byte-character>, i :: <small-integer>)
  %integer->character(i);
end method as;

define method as (ic == <integer>, ch :: <wide-character>)
  %wchar->integer(ch);
end method as;

define method as (cc == <wide-character>, i :: <small-integer>)
  %integer->wchar(i);
end method as;

define method as (ic == <integer>, ch :: <unicode-character>)
  %uchar->integer(ch);
end method as;

define method as (cc == <unicode-character>, i :: <small-integer>)
  %integer->uchar(i);
end method as;

//
// Case conversion
//

// as-lowercase

define method as-lowercase(c :: <byte-character>)
  %character-to-lowercase(c);
end method as-lowercase;

define method as-lowercase(c :: <wide-character>)
  %wchar-to-lowercase(c);
end method as-lowercase;

define method as-lowercase(c :: <unicode-character>)
  %uchar-to-lowercase(c);
end method as-lowercase;

// as-uppercase

define method as-uppercase(c :: <byte-character>)
  %character-to-uppercase(c);
end method as-uppercase;

define method as-uppercase(c :: <wide-character>)
  %wchar-to-uppercase(c);
end method as-uppercase;

define method as-uppercase(c :: <unicode-character>)
  %uchar-to-uppercase(c);
end method as-uppercase;

// as-titlecase

define method as-titlecase(c :: <unicode-character>)
  %uchar-to-uppercase(c);
end method as-titlecase;

//
// Character predicates
//

// alphabetic?

define method alphabetic? (c :: <byte-character>)
  %character-alphabetic?(c);
end method alphabetic?;

define method alphabetic? (c :: <wide-character>)
  %wchar-alphabetic?(c);
end method alphabetic?;

define method alphabetic? (c :: <unicode-character>)
  %uchar-alphabetic?(c);
end method alphabetic?;

// alphanumeric?

define method alphanumeric? (c :: <byte-character>)
  %character-alphanumeric?(c);
end method alphanumeric?;

define method alphanumeric? (c :: <wide-character>)
  %wchar-alphanumeric?(c);
end method alphanumeric?;

define method alphanumeric? (c :: <unicode-character>)
  %uchar-alphanumeric?(c);
end method alphanumeric?;

// control?

define method control? (c :: <byte-character>)
  %character-control?(c);
end method control?;

define method control? (c :: <wide-character>)
  %wchar-control?(c);
end method control?;

define method control? (c :: <unicode-character>)
  %uchar-control?(c);
end method control?;

// graphic?

define method graphic? (c :: <byte-character>)
  %character-graphic?(c);
end method graphic?;

define method graphic? (c :: <wide-character>)
  %wchar-graphic?(c);
end method graphic?;

define method graphic? (c :: <unicode-character>)
  %uchar-graphic?(c);
end method graphic?;

// printable?

define method printable? (c :: <byte-character>)
  %character-printable?(c);
end method printable?;

define method printable? (c :: <wide-character>)
  %wchar-printable?(c);
end method printable?;

define method printable? (c :: <unicode-character>)
  %uchar-printable?(c);
end method printable?;

// punctuation?

define method punctuation? (c :: <byte-character>)
  %character-punctuation?(c);
end method punctuation?;

define method punctuation? (c :: <wide-character>)
  %wchar-punctuation?(c);
end method punctuation?;

define method punctuation? (c :: <unicode-character>)
  %uchar-punctuation?(c);
end method punctuation?;

// whitespace?

define method whitespace? (c :: <byte-character>)
  %character-whitespace?(c);
end method whitespace?;

define method whitespace? (c :: <wide-character>)
  %wchar-whitespace?(c);
end method whitespace?;

define method whitespace? (c :: <unicode-character>)
  %uchar-whitespace?(c);
end method whitespace?;

// decimal?

define method decimal? (c :: <byte-character>)
  %character-decimal?(c);
end method decimal?;

define method decimal? (c :: <wide-character>)
  %wchar-decimal?(c);
end method decimal?;

define method decimal? (c :: <unicode-character>)
  %uchar-decimal?(c);
end method decimal?;

// hexadecimal?

define method hexadecimal? (c :: <byte-character>)
  %character-hexadecimal?(c);
end method hexadecimal?;

define method hexadecimal? (c :: <wide-character>)
  %wchar-hexadecimal?(c);
end method hexadecimal?;

define method hexadecimal? (c :: <unicode-character>)
  %uchar-hexadecimal?(c);
end method hexadecimal?;

// lowercase?

define method lowercase? (c :: <byte-character>)
  %character-lowercase?(c);
end method lowercase?;

define method lowercase? (c :: <wide-character>)
  %wchar-lowercase?(c);
end method lowercase?;

define method lowercase? (c :: <unicode-character>)
  %uchar-lowercase?(c);
end method lowercase?;

// uppercase?

define method uppercase? (c :: <byte-character>)
  %character-uppercase?(c);
end method uppercase?;

define method uppercase? (c :: <wide-character>)
  %wchar-uppercase?(c);
end method uppercase?;

define method uppercase? (c :: <unicode-character>)
  %uchar-uppercase?(c);
end method uppercase?;

// titlecase?

define method titlecase? (c :: <unicode-character>)
  %uchar-titlecase?(c);
end method uppercase?;

define method titlecase? (c :: <character>)
  %uchar-titlecase?(as(<unicode-character>, c));
end method uppercase?;
