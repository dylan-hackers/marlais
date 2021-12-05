module: dylan
copyright: (c) 2001, Marlais Hackers, LGPL (see "COPYRIGHT" file)

//
// init.dylan
//
// Revision History
// 08-28-2001 dma started file
//

%gc-disable();
%gc-expand(2621440);

// Basics
load("dylan/error.dylan");
load("dylan/object.dylan");
load("dylan/function.dylan");
load("dylan/comparison.dylan");
load("dylan/number.dylan");
load("dylan/character.dylan");
load("dylan/type.dylan");
load("dylan/functional.dylan");

// Collections
load("dylan/collection.dylan");
load("dylan/iteration.dylan");
load("dylan/list.dylan");
load("dylan/array.dylan");
load("dylan/vector.dylan");
load("dylan/string.dylan");
load("dylan/deque.dylan");
load("dylan/table.dylan");
load("dylan/range.dylan");

// Extensions
load("dylan/synchro.dylan");
load("dylan/bignum.dylan");
load("dylan/unicode.dylan");

%gc-enable();
