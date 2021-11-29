module: dylan

//
// string.dylan
//
// Authors:
//

//
// Generic methods
//

define method concatenate (s :: <string>, #rest more-strings)
 => (cs :: <string>);
  apply (concatenate-as, object-class(s), s, more-strings);
end method concatenate;

define method concatenate-as (c :: subclass(<string>), s :: <string>, #rest more-strings)
 => (cs :: <string>);
  apply (concatenate-as, c, as(c, s), more-strings);
end method concatenate-as;

//
// Methods on <byte-string>
//

define method element-type (s :: <byte-string>)
 => (type :: <type>);
  <byte-character>
end method;

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

define method concatenate-as (c == <byte-string>, s :: <byte-string>, #rest more-strings)
  if (empty? (more-strings))
    s
  else
    let n = as(<byte-string>, more-strings.head);
    let p = %string-append2(s, n);
    apply (concatenate-as, c, p, more-strings.tail);
  end if;
end method concatenate-as;

// XXX what is this for?
//define method as (ic == <integer>, s :: <string>)
//  let zero = as(<integer>, '0');
//  let total = 0;
//  local method accumulate (ch :: <character>)
//    total := 10 * total + (as(<small-integer>, ch) - zero);
//  end method;
//  do(accumulate, s);
//  total
//end method as;

//
// Methods on <wide-string>
//

define method element-type (s :: <wide-string>)
 => (type :: <type>);
  <wide-character>
end method;

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

define method concatenate-as (c == <wide-string>, s :: <wide-string>, #rest more-strings)
  if (empty? (more-strings))
    s
  else
    let n = as(<wide-string>, more-strings.head);
    let p = %wstring-append2(s, n);
    apply (concatenate-as, c, p, more-strings.tail);
  end if;
end method concatenate-as;

//
// Methods on <unicode-string>
//

define method element-type (s :: <unicode-string>)
 => (type :: <type>);
  <unicode-character>
end method;

define method element (s :: <unicode-string>, i :: <small-integer>,
		       #key default = %default-object)
  %ustring-element(s, i, default);
end method element;

define method element-setter (c :: <unicode-character>,
			      s :: <unicode-string>,
			      i :: <small-integer>)
  %ustring-element-setter (s, i, c);
end method element-setter;

define method size (s :: <unicode-string>)
  %ustring-size(s);
end method size;

define method concatenate-as (c == <unicode-string>, s :: <unicode-string>, #rest more-strings)
  if (empty? (more-strings))
    s
  else
    let n = as(<unicode-string>, more-strings.head);
    let p = %ustring-append2(s, n);
    apply (concatenate-as, c, p, more-strings.tail);
  end if;
end method concatenate-as;

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
// Iteration protocol - part specific to <unicode-string>
//

define method current-element (s :: <unicode-string>, state :: <integer-state>)
  %ustring-element(s, state, %default-object);
end method current-element;

define method current-element-setter (obj,
				      s :: <unicode-string>,
				      state :: <integer-state>)
  %ustring-element-setter(s, state, obj);
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

define method \< (s1 :: <unicode-string>, s2 :: <unicode-string>)
  %ustring<(s1, s2);
end method \<;

define method \= (s1 :: <unicode-string>, s2 :: <unicode-string>)
  %ustring=(s1, s2);
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

define method as-lowercase(s :: <wide-string>)
  %wstring-as-lowercase(s);
end method;

define method as-lowercase!(s :: <wide-string>)
  %wstring-as-lowercase!(s);
end method;

define method as-uppercase(s :: <wide-string>)
  %wstring-as-uppercase(s);
end method;

define method as-uppercase!(s :: <wide-string>)
  %wstring-as-uppercase!(s);
end method;

//
// String type conversion
//

define method as(cc == <byte-string>, ws :: <wide-string>)
  %wstring->bstring(ws);
end method as;

define method as(cc == <byte-string>, us :: <unicode-string>)
  %ustring->bstring(us);
end method as;

define method as(cc == <wide-string>, bs :: <byte-string>)
  %bstring->wstring(bs);
end method as;

define method as(cc == <wide-string>, us :: <unicode-string>)
  %ustring->wstring(us);
end method as;

define method as(cc == <unicode-string>, bs :: <byte-string>)
  %bstring->ustring(bs);
end method as;

define method as(cc == <unicode-string>, ws :: <wide-string>)
  %wstring->ustring(ws);
end method as;
