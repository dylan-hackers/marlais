module: dylan

// character.dylan
//
// Brent Benson
//

define sealed method \< (c1 :: <character>, c2 :: <character>)
  %character->integer(c1) < %character->integer(c2);
end method \<;

define sealed method as (ic == <small-integer>, ch :: <character>)
  %character->integer(ch);
end method as;

define sealed method as (cc == <character>, i :: <small-integer>)
  %integer->character(i);
end method as;

define sealed method as-lowercase(c :: <character>)
  %character-to-lowercase(c);
end method as-lowercase;

define sealed method as-uppercase(c :: <character>)
  %character-to-uppercase(c);
end method as-uppercase;
