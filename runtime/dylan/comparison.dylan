module: dylan

//
// comparison.dylan
//
// Authors:
//

define constant \~ =
  method(a)
      %not(a)
  end;

define constant \== =
  method(a, b)
      %identical?(a, b)
  end;

define constant \~== =
  method(a, b)
      %not(%identical?(a, b))
  end;

// according to DIRM, = should be a generic function so it can be extended
// by user classes. most primitive version just checks if they are ==.

define generic \= (o1 :: <object>, o2 :: <object>)
  => (equal? :: <boolean>);

define method \= (o1, o2)
  o1 == o2;
end method \=;

// \~= just calls \= and complements the result.

define constant \~= =
  method (o1, o2)
    ~(o1 = o2);
  end method;

// IRM definition:  < is a generic function.

define generic \< (o1 :: <object>, o2 :: <object>)
  => (o1-less-than-o2? :: <boolean>);

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
