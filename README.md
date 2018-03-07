# BSGenLib
C++ generic library extending the functionality provided primarily by Microsoft ATL/MFC libraries (but it can be used with some limitations also with STL). It was developed in 2006, thus lot of its functionality is now deprecated since in is offered by C++ 14+ but there is still some useful stuff.

## Features
* Getting min, max values of arrays of types supporting comparison operators
* Formatting time given in ms into the most approapriate form (e.g., 573 will be formatted as 573 ms, while 72535 as 01:12 min:sec)
* Opening console window (Windows only) from Win32 applications
* Converting between various types (similar to .NET Convert class)
* Delegates (as in C#)
* Manipulation with directories including searching for files/subdirectories (Windows only)
* Memory management to support fast dealocation
* Encapsulation of C enums into structs (different from enum class introduced in C++ 11)
* Reading / writing .INI files
* Managing large arrays not fitting the memory
* Logging routines
* Path manipulation
* Queue and priority queue
* Red-Black tree
* Support for counting object references
* Executing external applications
* Loading/saving images from/into BMP/BPNG/JPEG formats
* Various synchronization primitives
* Automated profiling of codes

## Recommended Use
\#include <altbase.h>   //to inform BSGenLib to derive the stuff from ATL classes

\#include <BSGenLib.h>
