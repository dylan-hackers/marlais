module: dylan-user
copyright: (c) 2001, LGPL, Douglas M. Auclair (see "copyright" file)

// source file history:
// 08-28-2001 dma started file

// we'll do libraries later
//define library io
//  use dylan;
//
//  export streams, print, standard-io;
//end library io;

define module streams
  use dylan;
  export <positionable-stream>, stream-position, stream-position-setter,
		adjust-stream-position, stream-size, stream-contents,
	 <buffered-stream>,
	 <sequence-stream>, <string-stream>, <byte-string-stream>,
		<unicode-string-stream>, type-for-sequence-stream, 
	 write-element, write, read-element, read;
// open-input-file, open-output-file, <file-stream>, close, eof-object?;
end module streams;

define module standard-io
  use dylan;
  use streams;

  export *standard-output*, *standard-input*, *standard-error*;
end module standard-io;

define module print
  use dylan;
  use streams;
  use standard-io;

  export print, print-object, print*, princ, object-princ, 
	 format, format-out;
end module print;
