module: dylan

//
// string.dylan
//


//
// Methods on <byte-string>
//

define method element (s :: <byte-string>, i :: <small-integer>,
		       #key default = %default-object)
  %string-element(s, i, default);
end method element;

define method element-setter (c :: <byte-character>,
			      s :: <byte-string>,
			      i :: <small-integer>)
  %string-element-setter (s, i, c);
end method element-setter;

define method size (s :: <byte-string>)
  %string-size(s);
end method size;

// XXX type safety on rest argument
// XXX implement string type conversion
define method concatenate (s :: <byte-string>, #rest more-strings)
  if (empty? (more-strings))
    s
  else
    apply (concatenate, pair (%string-append2(s, more-strings.head),
			      more-strings.tail));
  end if;
end method concatenate;

// XXX what is this for?
define method as (ic == <integer>, s :: <string>)
  let zero = as(<integer>, '0');
  let total = 0;
  local method accumulate (ch :: <character>)
    total := 10 * total + (as(<small-integer>, ch) - zero);
  end method;
  do(accumulate, s);
  total
end method as;


//
// Methods on <wide-string>
//

define method element (s :: <wide-string>, i :: <small-integer>,
		       #key default = %default-object)
  %wstring-element(s, i, default);
end method element;

define method element-setter (c :: <wide-character>,
			      s :: <wide-string>,
			      i :: <small-integer>)
  %wstring-element-setter (s, i, c);
end method element-setter;

define method size (s :: <wide-string>)
  %wstring-size(s);
end method size;


//
// Iteration protocol - generic part on <string>
//

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

define method copy-state (s :: <string>, state :: <integer-state>)
  state;
end method copy-state;


//
// Iteration protocol - part specific to <byte-string>
//

define method current-element (s :: <byte-string>, state :: <integer-state>)
  %string-element(s, state, %default-object);
end method current-element;

define method current-element-setter (obj,
				      s :: <byte-string>,
				      state :: <integer-state>)
  %string-element-setter(s, state, obj);
end method current-element-setter;


//
// Iteration protocol - part specific to <wide-string>
//

define method current-element (s :: <wide-string>, state :: <integer-state>)
  %wstring-element(s, state, %default-object);
end method current-element;

define method current-element-setter (obj,
				      s :: <wide-string>,
				      state :: <integer-state>)
  %wstring-element-setter(s, state, obj);
end method current-element-setter;


//
// Comparisons
//

define method \< (s1 :: <byte-string>, s2 :: <byte-string>)
  %string<(s1, s2);
end method \<;

define method \= (s1 :: <byte-string>, s2 :: <byte-string>)
  %string=(s1, s2);
end method \=;

define method \< (s1 :: <wide-string>, s2 :: <wide-string>)
  %wstring<(s1, s2);
end method \<;

define method \= (s1 :: <wide-string>, s2 :: <wide-string>)
  %wstring=(s1, s2);
end method \=;

//
// Case conversion
//

define method as-lowercase(s :: <byte-string>)
  %string-as-lowercase(s);
end method;

define method as-lowercase!(s :: <byte-string>)
  %string-as-lowercase!(s);
end method;

define method as-uppercase(s :: <byte-string>)
  %string-as-uppercase(s);
end method;

define method as-uppercase!(s :: <byte-string>)
  %string-as-uppercase!(s);
end method;

// define method as-lowercase(s :: <wide-string>)
//   %wstring-as-lowercase(s);
// end method;

// define method as-lowercase!(s :: <wide-string>)
//   %wstring-as-lowercase!(s);
// end method;

// define method as-uppercase(s :: <wide-string>)
//   %wstring-as-uppercase(s);
// end method;

// define method as-uppercase!(s :: <wide-string>)
//   %wstring-as-uppercase!(s);
// end method;
