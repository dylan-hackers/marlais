module: dylan
copyright: (c) 2001, LGPL, Douglas M. Auclair (see "copyright" file)

// print
// source file history:
// 08-28-2001 dma started file

define method print (obj)
  object-princ(*standard-output*, obj);
  %princ(*standard-output*, "\n");
end method print;

define method print-object (obj, stream)
  %print (stream, obj);
end method print-object;

define method print* (obj, #rest args)
  print (obj);
  unless (empty? (args))
    print* (args);
  end unless;
end method print*;

define method princ (obj)
  %princ (*standard-output*, obj);
end method princ;

define method object-princ (stream, obj)
  %princ (stream, obj);
end method object-princ;

define method write-number(s :: <stream>, d :: <integer>, #key base = 10)
  let hexi = "0123456789ABCDEF";
  let str-num = make(<deque>);
  while(d > base)
    let digit = modulo(d, base);
    push(str-num, hexi[digit]);
    d := floor/(d, base);
  end while;
  push(str-num, hexi[d]);
  write(s, as(<string>, str-num));
end method write-number;

define generic format-arg(s :: <stream>, fmt-char :: <character>, obj);
define method format-arg(s :: <stream>, fmt-char == 'd', obj)
  write-number(s, obj, 10);
end method format-arg;

define method format-arg(s :: <stream>, fmt-char == 'b', obj)
  write-number(s, obj, 2);
end method format-arg;

define method format-arg(s :: <stream>, fmt-char == 'o', obj)
  write-number(s, obj, 8);
end method format-arg;

define method format-arg(s :: <stream>, fmt-char == 'x', obj)
  write-number(s, obj, 16);
end method format-arg;

define method format-arg(s :: <stream>, fmt-char == 'c', obj)
  write-element(s, as(<character>, obj));
end method format-arg;

define method format-arg(s :: <stream>, fmt-char == '=', obj)
  print-object(obj, s);
end method format-arg;

// this may be really inefficient or really efficient, depending if
// the <sequence-stream> write is faster that writing elements to <stream>
// of unknown properties.
define method format (stream :: <stream>, s :: <string>, #rest args)
  let index = 0;
  let temp-stream = make(<sequence-stream>, 
			 contents: make(<deque>), direction: #"output");
  while(index < s.size)
    if(s[index] == '%')
      index := index + 1;
      if(s[index] == '%')
	write-element(temp-stream, '%');
      else
        format-arg(temp-stream, s[index], args.head);
        args := args.tail;
      end if;
    else
      write-element(temp-stream, s[index]);
    end if;
    index := index + 1;
  end while;
  write(stream, temp-stream.stream-contents);
end method format;

define constant format-out = curry(format, *standard-output*);
