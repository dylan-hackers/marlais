module: dylan

define abstract primary class <basic-stream> (<stream>)
  slot outer-stream :: <stream>,
    init-keyword: outer-stream:;
  slot stream-direction :: <stream-direction>,
    init-keyword: direction:;
  slot stream-element-type :: <type>,
    init-keyword: element-type:;
end class;
