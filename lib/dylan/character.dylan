module: dylan

// character.dylan
//
// Authors:
//   Brent Benson
//

//
// \<
//
// XXX replace these with a primitive
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
// as
//

// default implementation
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
// as-lowercase
//

define method as-lowercase(c :: <byte-character>)
  %character-to-lowercase(c);
end method as-lowercase;

define method as-lowercase(c :: <wide-character>)
  %wchar-to-lowercase(c);
end method as-lowercase;

define method as-lowercase(c :: <unicode-character>)
  %uchar-to-lowercase(c);
end method as-lowercase;

//
// as-uppercase
//

define method as-uppercase(c :: <byte-character>)
  %character-to-uppercase(c);
end method as-uppercase;

define method as-uppercase(c :: <wide-character>)
  %wchar-to-uppercase(c);
end method as-uppercase;

define method as-uppercase(c :: <unicode-character>)
  %uchar-to-uppercase(c);
end method as-uppercase;
