module: dylan

define generic character-name (c :: <character>)
  => (name :: <string>);

define method character-name (c :: <character>)
 => (name :: <string>);
  %uchar-name(as(<unicode-character>, c));
end method;

define method character-name (c :: <unicode-character>)
 => (name :: <string>);
  %uchar-name(c);
end method;

define generic unicode-block (c :: <character>)
  => (blk :: <integer>);

define method unicode-block (c :: <character>)
 => (blk :: <integer>);
  %uchar-block(as(<unicode-character>, c));
end method;

define method unicode-block (c :: <unicode-character>)
 => (blk :: <integer>);
  %uchar-block(c);
end method;
