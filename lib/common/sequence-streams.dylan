module: dylan
copyright: (c) 2001, LGPL, Douglas M. Auclair (see "copyright" file)

// source file history:
// 08-28-2001 dma started file
// 08-31-2001 dma added <stream>, as it's no longer built into the interpreter

//------------------------SEQUENCE-STREAMS-------------------------------

define abstract class <sequence-stream> (<positionable-stream>)
  slot pos, init-value: 0;
  slot contents, required-init-keyword: contents:;
  slot direction, init-keyword: direction:;
  slot start, init-keyword: start:;
  slot stop, init-keyword: end:;
end class <sequence-stream>;

define class <fixed-sequence-stream> (<sequence-stream>) end;
define class <stretchy-sequence-stream> (<sequence-stream>) end;
define abstract class <string-stream> (<fixed-sequence-stream>) end;
define class <byte-string-stream> (<string-stream>) end;
define class <wide-string-stream> (<string-stream>) end;
define class <unicode-string-stream> (<string-stream>) end;

define generic stream-contents(ps :: <sequence-stream>,
				#key clear-contents?);

define method stream-contents(ss :: <sequence-stream>, #key clear-contents?)
  let ans =
    if(clear-contents?)
      let temp = copy-sequence(ss.contents);
      ss.contents := make(type-for-copy(ss.contents));
      temp;
    else
      ss.contents;
    end;
  ans;
end method stream-contents;

define method stream-size(ps :: <positionable-stream>) => (s :: <integer>)
  ps.stream-contents.size;
end method stream-size;

define method stream-position(ps :: <sequence-stream>)
  ps.pos;
end method stream-position;

define method stream-position-setter(index, ps :: <sequence-stream>)
  if(index == #"start")
    ps.pos := 0;
  elseif(index == #"end")
    ps.pos := ps.stream-contents.stream-size - 1;
  else
    ps.pos := index;
  end if;
end method stream-position-setter;

define method adjust-stream-position(ps :: <sequence-stream>,
	delta :: <integer>, #key from = #"current")
  ps.pos := select(from)
    current: => ps.pos + delta;
    start: => delta;
    end: => ps.stream-contents.stream-size + delta;
  end;
end method adjust-stream-position;

define generic type-for-sequence-stream(seq :: <sequence>)
 => (ans :: <class>);

define method type-for-sequence-stream(seq :: <sequence>) => (ans :: <class>)
  <fixed-sequence-stream>;
end method type-for-sequence-stream;

define method type-for-sequence-stream(seq :: <stretchy-vector>)
 => (ans :: <class>)
  <stretchy-sequence-stream>;
end method type-for-sequence-stream;

define method type-for-sequence-stream(seq :: <deque>)
 => (ans :: <class>)
  <stretchy-sequence-stream>;
end method type-for-sequence-stream;

define method type-for-sequence-stream(seq :: <byte-string>)
 => (ans :: <class>)
  <byte-string-stream>;
end method type-for-sequence-stream;

define method type-for-sequence-stream(seq :: <wide-string>)
 => (ans :: <class>)
  <wide-string-stream>;
end method type-for-sequence-stream;

define method type-for-sequence-stream(seq :: <unicode-string>)
 => (ans :: <class>)
  <unicode-string-stream>;
end method type-for-sequence-stream;

define method make(ss == <sequence-stream>,
 #key contents, direction = #"input", start, end: stop)
  make(type-for-sequence-stream(contents),
	contents: contents, direction: direction, start: start, end: stop);
end method make;

define method make(ss == <string-stream>,
 #key contents, direction = #"input", start, end: stop)
  make(type-for-sequence-stream(contents),
	contents: contents, direction: direction, start: start, end: stop);
end method make;

define method initialize(ss :: <sequence-stream>,
  #key contents: cnt, direction: dir = #"input", start: beg = 0, end: stp)
  ss.contents := cnt;
  ss.direction := dir;
  ss.start := beg;
  ss.stop := if(stp) stp else cnt.size end;
end method initialize;

//------------------------READ/WRITE-------------------------------

define method check-direction(s :: <stream>, way :: <symbol>, fn :: <string>)
  unless(s.direction == #"input-output" | way == s.direction)
    error("Read/write error", s, s.direction, fn);
  end unless;
end method check-direction;

define method write-element(s :: <sequence-stream>, obj)
  check-direction(s, #"output", "write-element");
  if(s.stream-position >= s.stream-size)
    grow-stream(s, by: 1, elt: obj);
  else
    s.contents[s.stream-position] := obj;
  end if;
  adjust-stream-position(s, 1);
end method write-element;

define method grow-stream(s :: <fixed-sequence-stream>,
	 #key by = 1, elt = $unspecified)
  let orig = copy-sequence(s.contents);
  s.contents := make(type-for-copy(s.contents), size: s.stream-size + by);
  map-into(s.contents, identity, orig);
  if(elt ~== $unspecified) s.contents[s.stream-position] := elt; end if;
end method grow-stream;

define method grow-stream(s :: <stretchy-sequence-stream>,
	#key by = 1, elt = $unspecified)
  if(elt ~== $unspecified) do-grow(s.contents, elt); end if;
end method grow-stream;

define method do-grow(seq :: <stretchy-vector>, elt)
  add!(seq, elt);
end method do-grow;

define method do-grow(seq :: <deque>, elt)
  push-last(seq, elt);
end method do-grow;

define method write(s :: <sequence-stream>, seq :: <sequence>,
		    #key start = 0, end: stop = seq.size)
  check-direction(s, #"output", "write");
  let growth = 1 + seq.size + s.stream-position - s.stream-size;
  if(growth > 0) grow-stream(s, by: growth); end;
  for(x in seq) write-element(s, x) end;
end method write;

define method reader(s :: <sequence-stream>, n :: <integer>,
		     name :: <string>, copy-fn :: <function>, do-on-end)
  check-direction(s, #"input", name);
  let pos = s.stream-position;
  if(s.stream-size <= pos + n)
    if(do-on-end == $unspecified)
      error("Read past end of stream", s);
    else
      do-on-end;
    end if;
  else
    let ans = copy-fn(s.contents, pos, pos + n);
    adjust-stream-position(s, n);
    ans;
  end if;
end method reader;

define method read(s :: <sequence-stream>, n :: <integer>,
	#key on-end-of-stream = $unspecified)
  reader(s, n, "read",
	 method(seq, start, stop)
	  copy-sequence(seq, start: start, end: stop)
	 end, on-end-of-stream);
end method read;

define method read-element(s :: <sequence-stream>,
			   #key on-end-of-stream = $unspecified)
  reader(s, 1, "read-element", method(seq, start, stop) seq[start] end,
	 on-end-of-stream);
end method read-element;
