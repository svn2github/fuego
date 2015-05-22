/** @page generalportability Portability

C++ version:
Fuego is largely written using standard C++. The C++ version is C++03.
See http://en.wikipedia.org/wiki/C%2B%2B03
The current Fuego version does not use any features from the C++11 or C++14
standards. However, such a change is under consideration for future
releases.

External library dependency:
The only external library Fuego depends on is
<a href="http://www.boost.org/">Boost</a>. Boost is available on a variety
of platforms. Currently, Fuego requires Boost filesystem 3 
which is the only supported filesystem starting with Boost version
1.50, but should also compile with newer versions. Fuego has been tested with 
most recent Boost versions up to and including 1.58.0.
See www.boost.org/doc/libs/release/libs/filesystem for a filesystem reference.
The final Fuego version with support for filesystem 2 is revision 1983.

Non-standard functionality and support for specific platforms:
The implementation of functionality that is not available in the standard
C++ libraries or Boost is encapsulated in classes in the SmartGame library
(e.g. SgTime, SgProcess). The current implementation supports Windows and
systems supporting the POSIX standard. To port these classes to other
platforms, alternative implementations need to be added to the source code. */
