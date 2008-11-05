//----------------------------------------------------------------------------
/** @file FuegoMainUtil.h */
//----------------------------------------------------------------------------

#ifndef FUEGOMAIN_UTIL_H
#define FUEGOMAIN_UTIL_H

#include <string>

class GoBook;

//----------------------------------------------------------------------------

namespace FuegoMainUtil
{
    /** Try to load opening book from a set of known paths.
        The file name is "book.dat". The paths tried are (in this order):
        - the directory of the executable
        - ABS_TOP_SRCDIR/book
        - DATADIR/PACKAGE
        @param book The opening book to load
        @param programDir the directory of the executable
        @throws SgException, if book is not found
    */
    void LoadBook(GoBook& book, const std::string& programDir);

    /** Return Fuego version.
        If the macro VERSION was defined by the build system during compile
        time, its value will be used as the version, otherwise the version
        is "(__DATE__)". If compiled in debug mode, " (dbg)" will be added.
    */
    std::string Version();
}

//----------------------------------------------------------------------------

#endif // FUEGOMAIN_UTIL_H
