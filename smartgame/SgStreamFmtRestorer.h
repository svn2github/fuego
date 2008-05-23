//----------------------------------------------------------------------------
/** @file SgStreamFmtRestorer.h
    Restore a stream formatting state.
*/
//----------------------------------------------------------------------------

#ifndef SGSTREAMFMTRESTORER_H
#define SGSTREAMFMTRESTORER_H

#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------

/** Restore a stream formatting state.
    Automatically restores all formatting flags and parameters in its
    destructor.
*/
class SgStreamFmtRestorer
{
public:
    SgStreamFmtRestorer(std::ostream& out);

    ~SgStreamFmtRestorer();

private:
    std::ostream& m_out;

    /** Used temporarily for copying the formatting state to. */
    std::fstream m_dummy;
};

inline SgStreamFmtRestorer::SgStreamFmtRestorer(std::ostream& out)
    : m_out(out)
{
    m_dummy.copyfmt(out);
}

inline SgStreamFmtRestorer::~SgStreamFmtRestorer()
{
    m_out.copyfmt(m_dummy);
}

//----------------------------------------------------------------------------

#endif // SGSTREAMFMTRESTORER_H
