module: dylan

define constant <true> = singleton (#t);

define constant <false> = singleton (#f);

define method false-or (t :: <type>)
 => (fot :: <type>);
  type-union (type, <false>);
end method;
