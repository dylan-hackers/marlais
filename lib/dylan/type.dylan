module: dylan

define constant <true> = singleton(#t);

define constant <false> = singleton(#f);

define method false-or (type :: <type>)
 => (false-or-type :: <type>);
  type-union(type, <false>);
end method;
