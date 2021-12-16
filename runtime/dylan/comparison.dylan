module: dylan

//
// comparison.dylan
//
// Authors:
//

// Magnitude comparison

define generic \= (o1 :: <object>, o2 :: <object>)
  => (equal? :: <boolean>);

define generic \< (o1 :: <object>, o2 :: <object>)
  => (o1-less-than-o2? :: <boolean>);

// Default implementations

define method \= (o1, o2)
  o1 == o2;
end method \=;

define method \< (o1, o2)
  error("\<: Can't compare these objects", o1, o2);
end method \<;

// Derived cases

define constant \~= =
  method (o1, o2)
    ~(o1 = o2);
  end method;

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
