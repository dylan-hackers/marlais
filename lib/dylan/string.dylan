module: dylan

//
// string.dylan
//

define method element (s :: <string>, i :: <small-integer>,
		       #key default = %default-object)
  %string-element(s, i, default);
end method element;

define method element-setter (c :: <character>,
			      s :: <string>,
			      i :: <small-integer>)
  %string-element-setter (s, i, c);
end method element-setter;

define method size (s :: <string>)
  %string-size(s);
end method size;

define method concatenate (s :: <string>, #rest more-strings)
  if (empty? (more-strings))
    s
  else
    apply (concatenate, pair (%string-append2(s, more-strings.head),
			      more-strings.tail));
  end if;
end method concatenate;

define method as (ic == <small-integer>, s :: <string>)
  let zero = as(<small-integer>, '0');
  let total = 0;
  local method accumulate (ch :: <character>)
    total := 10 * total + (as(<small-integer>, ch) - zero);
  end method;
  do(accumulate, s);
  total
end method as;

//
// iteration protocol
//

define constant <integer-state> = union (<small-integer>, singleton(#f));

define method current-key (s :: <collection>, state :: <integer-state>)
  state
end method current-key;

define method initial-state (s :: <string>)
  if (size(s) = 0) #f; else 0; end if;
end method initial-state;

define method next-state (s :: <string>, state :: <integer-state>)
  if (state < s.size - 1)
    state + 1;
  else
    #f;
  end if;
end method next-state;

define method current-element (s :: <string>, state :: <integer-state>)
  %string-element(s, state, %default-object);
end method current-element;

define method current-element-setter (obj,
				      s :: <string>,
				      state :: <integer-state>)
  %string-element-setter(s, state, obj);
end method current-element-setter;

define method copy-state (s :: <string>, state :: <integer-state>)
  state;
end method copy-state;

// comparisons

define method \< (s1 :: <string>, s2 :: <string>)
  %string<(s1, s2);
end method \<;

define method \= (s1 :: <string>, s2 :: <string>)
  %string=(s1, s2);
end method \=;

// case conversion

define method as-lowercase(s :: <string>)
  %string-as-lowercase(s);
end method;

define method as-lowercase!(s :: <string>)
  %string-as-lowercase!(s);
end method;

define method as-uppercase(s :: <string>)
  %string-as-uppercase(s);
end method;

define method as-uppercase!(s :: <string>)
  %string-as-uppercase!(s);
end method;
