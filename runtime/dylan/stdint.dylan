module: dylan

// stdint.dylan
//
// Authors:
//   Ingo Albrecht
//

// <byte>

define constant $byte-bits = 8;
define constant $byte-size = 1;
define constant $minimum-byte = 0;
define constant $maximum-byte = 255;
define constant <byte> =
  limited(<integer>, min: $minimum-byte, max: $maximum-byte);

// <int8>

define constant $int8-bits = 8;
define constant $int8-size = 1;
define constant $minimum-int8 = -128;
define constant $maximum-int8 = 127;
define constant <int8> =
  limited(<integer>, min: $minimum-int8, max: $maximum-int8);

// <uint8>

define constant $uint8-bits = 8;
define constant $uint8-size = 1;
define constant $minimum-uint8 = 0;
define constant $maximum-uint8 = 255;
define constant <uint8> =
  limited(<integer>, min: $minimum-uint8, max: $maximum-uint8);

// <int16>

define constant $int16-bits = 16;
define constant $int16-size = 2;
define constant $minimum-int16 = -32768;
define constant $maximum-int16 = 32767;
define constant <int16> =
  limited(<integer>, min: $minimum-int16, max: $maximum-int16);

// <uint16>

define constant $uint16-bits = 16;
define constant $uint16-size = 2;
define constant $minimum-uint16 = 0;
define constant $maximum-uint16 = 65535;
define constant <uint16> =
  limited(<integer>, min: $minimum-uint16, max: $maximum-uint16);
