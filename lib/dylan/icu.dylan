module: dylan

define generic character-name (c :: <character>)
  => (name :: <string>);

define method character-name (c :: <character>)
 => (name :: <string>);
  %icu-char-name(as(<unicode-character>, c));
end function;

define method character-name (c :: <unicode-character>)
 => (name :: <string>);
  %icu-char-name(c);
end function;

define generic unicode-block (c :: <character>)
  => (blk :: <integer>);

define method unicode-block (c :: <character>)
 => (blk :: <integer>);
  %icu-char-block(as(<unicode-character>, c));
end function;

define method unicode-block (c :: <unicode-character>)
 => (blk :: <integer>);
  %icu-char-block(c);
end function;
