module: dylan

define abstract primary class <basic-stream> (<stream>)
  slot outer-stream :: false-or(<stream>),
    init-value: #f,
    init-keyword: outer-stream:;
  slot stream-direction :: <stream-direction>,
    init-value: #"input",
    init-keyword: direction:;
end class;
