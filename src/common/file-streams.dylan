module: dylan
copyright: (c) 2001, LGPL, Marlais Hackers (see COPYRIGHT file for use)

// module: file-streams
// source file history:
// 08-28-2001 dma started file
  
//------------------------FILE-DESCRIPTORS-------------------------------
define class <fd-stream> (<stream>)
  slot file-descriptor, required-init-keyword: fd:;
  slot fd-direction, init-keyword: direction:, init-value: #"input";
end class <fd-stream>;

define method initialize(stream :: <fd-stream>, 
	#key direction = #"input", fd) => (ans :: <fd-stream>)
  next-method();
  stream.fd-direction := direction;
  stream.file-descriptor := fd;
  stream;
end method initialize;

define class <file-stream> (<buffered-stream>, <positionable-stream>)
  slot name, required-init-keyword: locator:;
  slot fd :: <fd-stream>; // initialized in make
end class <file-stream>;

// need specialized methods for <file-stream> from <positionable-stream>

/*** um, we'll do this later!
define method initialize(file :: <file-stream>,
	#key direction = #"input", locator) => (opened-file :: <file-stream>)
  let rw = select(direction)
	    (#"input") => $O_RDONLY;
	    (#"output") => $O_WRONLY;
	    (#"input-output") => $O_RDWR;
	    otherwise => error("Bad value to make <file-stream>", direction);
	   end;
  unless(locator) error("Must open a file with a name!", locator); end;
  let fd-id = call-out("open", #"int", string: locator, int: rw, int: 0);
  file.fd := make(<fd-stream>, fd: fd-id, direction: direction);
  file.name := locator;
  file;
end method initialize;
 */

// define method open-input-file (s :: <string>)
//   %open-input-file(s);
// end method open-input-file;

// define method open-output-file (s :: <string>)
//   %open-output-file(s);
// end method open-output-file;

define method close (s :: <fd-stream>)
  %close-stream(s.file-descriptor);
end method close;

define method eof-object? (obj)
  %eof-object?(obj);
end method eof-object?;

// so, for FD's write is more efficient that write-element
define method write-element (s :: <fd-stream>, c :: <character>)
  let str = make(<string>, size: 1, fill: c);
  write(s, str);
end method write-element;

define method write(s :: <fd-stream>, str :: <string>, 
	#key start = 0, end: stop = str.size)
  let seq = copy-sequence(str, start: start, end: stop);
// maybe later or never:  let buf = as(<buffer>, seq);
  %write(s.file-descriptor, seq);
end method write;

/*** need to invent <buffer>s for this to work
define method read(s :: <fd-stream>, n :: <integer>, 
		   #key on-end-of-stream = $unspecified)
  let str = make(<buffer>, size: n);
  let len = %read(s.fd, str, n);
  if(len < n)
    if(on-end-of-stream == $unspecified)
      error("Read beyond the end of the stream", s);
    else
      on-end-of-stream;
    end if;
  else
    as(<string>, str);
  end if;
end method read;

// Here, again, read is more efficient that read-element for FD's
define method read-element(s :: <fd-stream>, 
			   #key on-end-of-stream = $unspecified)
  let str = read(s, 1, on-end-of-stream: on-end-of-stream);
  str[0];
end method read-element;
 */