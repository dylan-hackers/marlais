module: dylan

define generic character-name (c :: <character>)
  => (name :: <string>);

define method character-name (c :: <character>)
 => (name :: <string>);
  %uchar-name(as(<unicode-character>, c));
end function;

define method character-name (c :: <unicode-character>)
 => (name :: <string>);
  %uchar-name(c);
end function;

define generic unicode-block (c :: <character>)
  => (blk :: <integer>);

define method unicode-block (c :: <character>)
 => (blk :: <integer>);
  %uchar-block(as(<unicode-character>, c));
end function;

define method unicode-block (c :: <unicode-character>)
 => (blk :: <integer>);
  %uchar-block(c);
end function;
