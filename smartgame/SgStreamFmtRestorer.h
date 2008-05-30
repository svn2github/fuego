//----------------------------------------------------------------------------
/** @file SgStreamFmtRestorer.h
    Restore a stream formatting state.
*/
//----------------------------------------------------------------------------

#ifndef SG_STREAMFMTRESTORER_H
#define SG_STREAMFMTRESTORER_H

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

#endif // SG_STREAMFMTRESTORER_H
