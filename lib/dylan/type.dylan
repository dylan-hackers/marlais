module: dylan

define constant <true> = singleton (#t);

define constant <false> = singleton (#f);

define method false-or (t :: <type>)
 => (fot :: <type>);
  type-union (type, <false>);
end method;

define method one-of (#rest things)
 => (one-of-type :: <type>);
  apply (type-union, map (singleton, things));
end method;
