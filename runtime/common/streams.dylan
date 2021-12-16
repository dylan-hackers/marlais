module: dylan

// TODO better constant?
define constant $unspecified = #"not supplied";

//
// Stream directions
//

define constant <stream-direction>
  = one-of(#"input", #"output", #"input-output");

//
// Stream positions
//

// Any object can be a stream position
//
// The specification only allows <integer> and an
// abstract class called <stream-position>, but this
// definition yields much the same result.
//
define constant <stream-position> = <object>;

//
// Stream classes
//

define abstract class <stream> (<object>)
end class;

define abstract class <buffered-stream> (<stream>)
end class;

define abstract class <positionable-stream> (<stream>)
end class;

//
// Basic methods
//

define generic stream-direction (stream :: <stream>)
  => (d :: <stream-direction>);

define generic stream-element-type (stream :: <stream>)
  => (t :: <object>);

define generic stream-sequence-class (stream :: <stream>)
  => (c :: <class>);

define generic stream-open? (stream :: <stream>)
  => (o :: <boolean>);

define generic stream-size (stream :: <stream>)
  => (c :: false-or(<integer>));

define generic close (stream :: <stream>)
  => ();

define method stream-open? (stream :: <stream>)
 => (o :: <boolean>);
  #t
end method;

define method stream-size (stream :: <stream>)
  => (c :: false-or(<integer>));
  #f
end method;

define method close (stream :: <stream>)
 => ();
  #f
end method;

//
// Wrapper support
//

define open generic outer-stream (stream :: <stream>)
  => (wrapper :: <stream>);

define open generic outer-stream-setter (wrapper :: <stream>, stream :: <stream>)
  => (wrapper :: <stream>);

//
// Locking
//

define generic stream-locked? (stream :: <stream>)
  => (o :: <boolean>);

define generic lock-stream (stream :: <stream>)
  => ();

define generic unlock-stream (stream :: <stream>)
  => ();

//
// Reading
//

define generic read (stream :: <stream>, n :: <integer>,
                     #rest keys, #key on-end-of-stream)
  => (sequence-or-eof :: <object>);

define generic read-into! (stream :: <stream>, n :: <integer>, s :: <sequence>,
                           #rest keys, #key start, on-end-of-stream)
  => (n-read-or-eof :: <object>);

define generic read-element (stream :: <stream>,
                             #rest keys, #key on-end-of-stream)
  => (element-or-eof :: <object>);

//
// Peeking
//

define generic peek (stream :: <stream>, n :: <integer>,
                     #rest keys, #key on-end-of-stream)
  => (element-or-eof :: <object>);

define generic stream-input-available? (stream :: <stream>)
  => (available? :: <boolean>);

define generic unread-element (stream :: <stream>, e :: <object>)
  => (element :: <object>);

//
// Writing
//

define generic write (stream :: <stream>, s :: <sequence>,
                      #rest keys, #key start, end: stop)
  => ();

define generic write-element (stream :: <stream>, e :: <object>)
  => ();

//
// Positioning
//

define open generic stream-position
  (stream :: <positionable-stream>)
  => (position :: <stream-position>);

define open generic stream-position-setter
  (position :: <stream-position>, stream :: <positionable-stream>)
  => (position :: <object>);

define open generic adjust-stream-position
  (stream :: <positionable-stream>, delta :: <integer>, #key from)
  => (position :: <stream-position>);

//
// Flushing and synchronization
//

define generic discard-input (stream :: <stream>)
  => ();

define generic discard-output (stream :: <stream>)
  => ();

define generic force-output (stream :: <stream>, #key synchronize?)
  => ();

define generic synchronize-output (stream :: <stream>)
  => ();

define method discard-input (stream :: <stream>)
 => ();
  #f
end method;

define method discard-output (stream :: <stream>)
 => ();
  #f
end method;

define method force-output (stream :: <stream>, #key synchronize?)
 => ();
  #f
end method;

define method synchronize-output (stream :: <stream>)
 => ();
  force-output (stream, synchronize?: #t);
end method;
