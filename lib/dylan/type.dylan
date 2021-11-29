module: dylan

//
// type.dylan
//
// Authors:
//   Ingo Albrecht
//

//
// Boolean singletons
//

define constant <true> = singleton (#t);

define constant <false> = singleton (#f);

//
// Convenience types
//

define method false-or (type :: <type>)
 => (false-or-type :: <type>);
  type-union (type, <false>);
end method;

define method one-of (#rest things)
 => (one-of-type :: <type>);
  apply (type-union, map (singleton, things));
end method;
