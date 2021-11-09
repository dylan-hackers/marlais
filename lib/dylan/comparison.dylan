module: dylan

//
// comparisons.
//

define constant \~ = method(a) %not(a) end;

define constant \== = method(a, b) %identical?(a, b) end;
define constant \~== = method(a, b) %not(%identical?(a, b)) end;

// according to DIRM, = should be a generic function so it can be extended
// by user classes. most primitive version just checks if they are ==.

define method \= (o1, o2)
  o1 == o2;
end method \=;

// \~= just calls \= and complements the result.

define constant \~= =
  method (o1, o2)
    ~(o1 = o2);
  end method;

// IRM definition:  < is a generic function.

define method \< (o1, o2)
  error("\<: Can't compare these objects", o1, o2);
end method \<;

// >, <=, and >= are defined by <.

define constant \> =
  method (o1, o2)
    o2 < o1;
  end method;

define constant \<= =
  method (o1, o2)
    ~(o2 < o1);
  end method;

define constant \>= =
  method (o1, o2)
    ~(o1 < o2);
  end method;

// comparisons

define method \< (c1 :: <character>, c2 :: <character>)
  as(<integer>, c1) < as(<integer>, c2);
end method \<;
