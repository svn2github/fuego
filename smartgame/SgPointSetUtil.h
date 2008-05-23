//----------------------------------------------------------------------------
/** @file SgPointSetUtil.h
    Utility functions for SgPointSet.
*/
//----------------------------------------------------------------------------

#ifndef SGPOINTSETUTIL_H
#define SGPOINTSETUTIL_H

#include <iosfwd>
#include <string>

class SgPointSet;

//----------------------------------------------------------------------------

/** Write all points in set. */
class SgWritePointSet
{
public:
    SgWritePointSet(const SgPointSet& pointSet, std::string label = "",
                    bool writeSize = true);

    std::ostream& Write(std::ostream& out) const;

private:
    bool m_writeSize;

    const SgPointSet& m_pointSet;

    std::string m_label;
};

/** @relatesalso SgWritePointSet */
std::ostream& operator<<(std::ostream& out, const SgWritePointSet& write);

/** @relatesalso SgPointSet */
std::ostream& operator<<(std::ostream& out, const SgPointSet& set);

//----------------------------------------------------------------------------

/** Write center point and size of set */
class SgWritePointSetID
{
public:
    explicit SgWritePointSetID(const SgPointSet& p)
        : m_p(p)
    { }

    const SgPointSet& Points() const { return m_p; }
private:

    const SgPointSet& m_p;
};

std::ostream& operator<<(std::ostream& stream, const SgWritePointSetID& w);

//----------------------------------------------------------------------------

/** Read all points in set. */
class SgReadPointSet
{
public:
    SgReadPointSet(SgPointSet& pointSet);

    std::istream& Read(std::istream& in) const;

private:
    mutable SgPointSet& m_pointSet; // allow temp objects to modify
};

/** @relatesalso SgReadPointSet */
std::istream& operator>>(std::istream& in, const SgReadPointSet& Read);

std::istream& operator>>(std::istream& in, SgPointSet& pointSet);

//----------------------------------------------------------------------------

#endif // SGPOINTSETUTIL_H

