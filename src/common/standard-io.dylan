module: dylan
copyright: (c) 2001, LGPL, Douglas M. Auclair (see "copyright" file)

// standard-io
// source file history:
// 08-28-2001 dma started file

//define constant *standard-input* :: <fd-stream>
//  = make(<fd-stream>, fd: 0);

//define constant *standard-output* :: <fd-stream>
//  = make(<fd-stream>, fd: 1, direction: #"output");

//define constant *standard-error* :: <fd-stream>
//  = make(<fd-stream>, fd: 2, direction: #"output");

define variable *standard-output* :: <stream>
  = %standard-output();
define variable *standard-error* :: <stream> 
  = %standard-error();
define variable *standard-input* :: <stream>
  = %standard-input();
