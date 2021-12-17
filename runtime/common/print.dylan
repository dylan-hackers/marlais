module: dylan
copyright: (c) 2001, LGPL, Marlais Hackers (see "COPYRIGHT" file)

// print
// source file history:
// 08-28-2001 dma started file

define method print (obj)
  object-princ(*standard-output*, obj);
  %princ(*standard-output*.file-descriptor, "\n");
end method print;

define method print-object (obj, stream)
  %print (stream.file-descriptor, obj);
end method print-object;

define method print* (obj, #rest args)
  print (obj);
  unless (empty? (args))
    print* (args);
  end unless;
end method print*;

define method princ (obj)
  %princ (*standard-output*.file-descriptor, obj);
end method princ;

define method object-princ (stream, obj)
  %princ (stream.file-descriptor, obj);
end method object-princ;

// I think this can be safely moved to the interpreter
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

// all these format args need to be fixed for uppercase format args!
define method format-arg(s :: <stream>, fmt-char == 'd', obj)
  write-number(s, obj, base: 10);
end method format-arg;

define method format-arg(s :: <stream>, fmt-char == 'b', obj)
  write-number(s, obj, base: 2);
end method format-arg;

define method format-arg(s :: <stream>, fmt-char == 'o', obj)
  write-number(s, obj, base: 8);
end method format-arg;

define method format-arg(s :: <stream>, fmt-char == 'x', obj)
  write-number(s, obj, base: 16);
end method format-arg;

define method format-arg(s :: <stream>, fmt-char == 'c', obj)
  write-element(s, as(<character>, obj));
end method format-arg;

define method format-arg(s :: <stream>, fmt-char == '=', obj)
  print-object(obj, s);
end method format-arg;

define method format-arg(s :: <stream>, fmt-char == 's', obj)
  write(s, obj);
end method format-arg;

// This is about the best for speed's sake that I can do.  One character
// at a time is painfully slow, and writing to an interem stream isn't
// much better.  Once Marlais gets a really turbo-charged intepreter
// algorithm, this'll be faster than C (-; -- dma
define method format (stream :: <stream>, s :: <string>, #rest args) => ()
  let index = 0;
  let old-index = 0;

  while(index < s.size)
    if(s[index] == '%')
      write(stream, s, start: old-index, end: index);
      index := index + 1;
      if(s[index] == '%') // I hate this case, but c'est la vie -- dma
	write-element(stream, '%');
      else
        format-arg(stream, s[index], args.head);
        args := args.tail;
      end if;
      old-index := index + 1;
    end if;
    index := index + 1;
  end while;
  write(stream, s, start: old-index, end: index);
end method format;

define constant format-out = curry(format, *standard-output*);
