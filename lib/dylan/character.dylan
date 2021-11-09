module: dylan

//
// characters
//

//
// character.dyl
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

begin
  let uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  let lowercase = "abcdefghijklmnopqrstuvwxyz";

  define sealed method as-lowercase(c :: <character>)
    let i = find-key(uppercase,curry(\=,c));
    if (i)
      lowercase[i]
    else
      c
    end
  end method as-lowercase;

  define sealed method as-uppercase(c :: <character>)
    let i = find-key(lowercase,curry(\=,c));
    if (i)
      uppercase[i]
    else
      c
    end
  end method as-uppercase;
end;
