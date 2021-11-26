module: dylan

//
// Stream classes
//

define abstract class <stream> (<object>)
end class;

define abstract class <buffered-stream> (<stream>)
end class;

define abstract class <positionable-stream> (<stream>)
end class;

// TODO better constant?
define constant $unspecified = #"not supplied";

//
// Methods on <stream>
//

define generic read(s :: <stream>, n :: <integer>, #key on-end-of-stream);
define generic read-element(s :: <stream>, #key on-end-of-stream);
define generic write(s :: <stream>, seq :: <sequence>, #key start, end: stop);
define generic write-element(s :: <stream>, obj);

//
// Methods on <positionable-stream>
//

define generic stream-position(ps :: <positionable-stream>)
  => (position);
define generic stream-position-setter(index, ps :: <positionable-stream>)
  => (index);
