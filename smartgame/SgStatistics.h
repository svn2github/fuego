//----------------------------------------------------------------------------
/** @file SgStatistics.h
    Classes for computing mean, variances.
    Note that the classes SgStatisticsBase, SgStatistics, and SgStatisticsExt
    derive from each other for convenience of implementation only, they don't
    use virtual functions for efficiency and are not meant to be used
    polymorphically.
*/
//----------------------------------------------------------------------------

#ifndef SG_STATISTICS_H
#define SG_STATISTICS_H

#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include "SgException.h"
#include "SgWrite.h"

//----------------------------------------------------------------------------

/** Computes mean of a statistical variable.
    The template parameters are the floating point type and the counter type,
    depending on the precision-memory tradeoff.
*/
template<typename FLOAT, typename INT>
class SgStatisticsBase
{
public:
    SgStatisticsBase();

    /** Create statistics initialized with values.
        Equivalent to creating a statistics and calling @c count times
        Add(val)
    */
    SgStatisticsBase(FLOAT val, INT count);

    void Add(FLOAT val);

    void Clear();

    INT Count() const;

    /** Initialize with values.
        Equivalent to calling Clear() and calling @c count times
        Add(val)
    */
    void Initialize(FLOAT val, INT count);

    FLOAT Mean() const;

    /** Write in human readable format. */
    void Write(std::ostream& out) const;

    /** Save in a compact platform-independent text format.
        The data is written in a single line, without trailing newline.
    */
    void SaveAsText(std::ostream& out) const;

    /** Load from text format.
        See SaveAsText()
    */
    void LoadFromText(std::istream& in);

private:
    INT m_count;

    FLOAT m_mean;
};

template<typename FLOAT, typename INT>
SgStatisticsBase<FLOAT,INT>::SgStatisticsBase()
{
    Clear();
}

template<typename FLOAT, typename INT>
SgStatisticsBase<FLOAT,INT>::SgStatisticsBase(FLOAT val, INT count)
{
    Initialize(val, count);
}

template<typename FLOAT, typename INT>
void SgStatisticsBase<FLOAT,INT>::Add(FLOAT val)
{
    // Write order dependency: at least on class (SgUctSearch in lock-free
    // mode) uses SgStatisticsBase concurrently without locking and assumes
    // that m_mean is valid, if m_count is greater zero
    INT count = m_count;
    ++count;
    SG_ASSERT(count > 0); // overflow
    val -= m_mean;
    m_mean +=  val / count;
    m_count = count;
}

template<typename FLOAT, typename INT>
void SgStatisticsBase<FLOAT,INT>::Clear()
{
    m_count = 0;
    m_mean = 0;
}

template<typename FLOAT, typename INT>
INT SgStatisticsBase<FLOAT,INT>::Count() const
{
    return m_count;
}

template<typename FLOAT, typename INT>
void SgStatisticsBase<FLOAT,INT>::Initialize(FLOAT val, INT count)
{
    m_count = count;
    m_mean = val;
}

template<typename FLOAT, typename INT>
void SgStatisticsBase<FLOAT,INT>::LoadFromText(std::istream& in)
{
    in >> m_count >> m_mean;
}

template<typename FLOAT, typename INT>
FLOAT SgStatisticsBase<FLOAT,INT>::Mean() const
{
    return m_mean;
}

template<typename FLOAT, typename INT>
void SgStatisticsBase<FLOAT,INT>::Write(std::ostream& out) const
{
    if (m_count == 0)
        out << '-';
    else
        out << Mean();
}

template<typename FLOAT, typename INT>
void SgStatisticsBase<FLOAT,INT>::SaveAsText(std::ostream& out) const
{
    out << m_count << ' ' << m_mean;
}

//----------------------------------------------------------------------------

/** Computes mean and variance of a statistical variable.
    The template parameters are the floating point type and the counter type,
    depending on the precision-memory tradeoff.
*/
template<typename FLOAT, typename INT>
class SgStatistics
    : public SgStatisticsBase<FLOAT,INT>
{
public:
    SgStatistics();

    /** Create statistics initialized with values.
        Equivalent to creating a statistics and calling @c count times
        Add(val)
    */
    SgStatistics(FLOAT val, INT count);

    void Add(FLOAT val);

    void Clear();

    FLOAT Deviation() const;

    FLOAT Variance() const;

    /** Write in human readable format. */
    void Write(std::ostream& out) const;

    /** Save in a compact platform-independent text format.
        The data is written in a single line, without trailing newline.
    */
    void SaveAsText(std::ostream& out) const;

    /** Load from text format.
        See SaveAsText()
    */
    void LoadFromText(std::istream& in);

private:
    FLOAT m_variance;
};

template<typename FLOAT, typename INT>
SgStatistics<FLOAT,INT>::SgStatistics()
{
    Clear();
}

template<typename FLOAT, typename INT>
SgStatistics<FLOAT,INT>::SgStatistics(FLOAT val, INT count)
    : SgStatisticsBase<FLOAT,INT>(val, count)
{
    m_variance = 0;
}

template<typename FLOAT, typename INT>
void SgStatistics<FLOAT,INT>::Add(FLOAT val)
{
    FLOAT meanOld = SgStatisticsBase<FLOAT,INT>::Mean();
    INT countOld = SgStatisticsBase<FLOAT,INT>::Count();
    SgStatisticsBase<FLOAT,INT>::Add(val);
    FLOAT mean = SgStatisticsBase<FLOAT,INT>::Mean();
    INT count = SgStatisticsBase<FLOAT,INT>::Count();
    m_variance = (countOld * (m_variance + meanOld * meanOld)
                  + val * val) / count  - mean * mean;
}

template<typename FLOAT, typename INT>
void SgStatistics<FLOAT,INT>::Clear()
{
    SgStatisticsBase<FLOAT,INT>::Clear();
    m_variance = 0;
}

template<typename FLOAT, typename INT>
FLOAT SgStatistics<FLOAT,INT>::Deviation() const
{
    return std::sqrt(m_variance);
}

template<typename FLOAT, typename INT>
void SgStatistics<FLOAT,INT>::LoadFromText(std::istream& in)
{
    SgStatisticsBase<FLOAT,INT>::LoadFromText(in);
    in >> m_variance;
}

template<typename FLOAT, typename INT>
FLOAT SgStatistics<FLOAT,INT>::Variance() const
{
    return m_variance;
}

template<typename FLOAT, typename INT>
void SgStatistics<FLOAT,INT>::Write(std::ostream& out) const
{
    if (SgStatisticsBase<FLOAT,INT>::Count() == 0)
        out << '-';
    else
        out << SgStatisticsBase<FLOAT,INT>::Mean() << " dev=" << Deviation();
}

template<typename FLOAT, typename INT>
void SgStatistics<FLOAT,INT>::SaveAsText(std::ostream& out) const
{
    SgStatisticsBase<FLOAT,INT>::SaveAsText(out);
    out << ' ' << m_variance;
}

//----------------------------------------------------------------------------

/** Extended version of SgStatistics.
    Also stores minimum and maximum values.
    The template parameters are the floating point type and the counter type,
    depending on the precision-memory tradeoff.
*/
template<typename FLOAT, typename INT>
class SgStatisticsExt
    : public SgStatistics<FLOAT,INT>
{
public:
    SgStatisticsExt();

    void Add(FLOAT val);

    void Clear();

    FLOAT Max() const;

    FLOAT Min() const;

    void Write(std::ostream& out) const;

private:
    FLOAT m_max;

    FLOAT m_min;
};

template<typename FLOAT, typename INT>
SgStatisticsExt<FLOAT,INT>::SgStatisticsExt()
{
    Clear();
}

template<typename FLOAT, typename INT>
void SgStatisticsExt<FLOAT,INT>::Add(FLOAT val)
{
    SgStatistics<FLOAT,INT>::Add(val);
    if (val > m_max)
        m_max = val;
    if (val < m_min)
        m_min = val;
}

template<typename FLOAT, typename INT>
void SgStatisticsExt<FLOAT,INT>::Clear()
{
    SgStatistics<FLOAT,INT>::Clear();
    m_min = std::numeric_limits<FLOAT>::max();
    m_max = -std::numeric_limits<FLOAT>::max();
}

template<typename FLOAT, typename INT>
FLOAT SgStatisticsExt<FLOAT,INT>::Max() const
{
    return m_max;
}

template<typename FLOAT, typename INT>
FLOAT SgStatisticsExt<FLOAT,INT>::Min() const
{
    return m_min;
}

template<typename FLOAT, typename INT>
void SgStatisticsExt<FLOAT,INT>::Write(std::ostream& out) const
{
    if (SgStatistics<FLOAT,INT>::Count() == 0)
        out << '-';
    else
    {
        SgStatistics<FLOAT,INT>::Write(out);
        out << " min=" << m_min << " max=" << m_max;
    }
}

//----------------------------------------------------------------------------

/** Set of named statistical variables.
    The template parameters are the floating point type and the counter type,
    depending on the precision-memory tradeoff.
*/
template<typename FLOAT, typename INT>
class SgStatisticsCollection
{
public:
    /** Add the statistics of another collection.
        The collections must contain the same entries.
    */
    void Add(const SgStatisticsCollection<FLOAT,INT>& collection);

    void Clear();

    bool Contains(const std::string& name) const;

    /** Create a new variable. */
    void Create(const std::string& name);

    const SgStatistics<FLOAT,INT>& Get(const std::string& name) const;

    SgStatistics<FLOAT,INT>& Get(const std::string& name);

    void Write(std::ostream& o) const;

private:
    typedef std::map<std::string,SgStatistics<FLOAT,INT> > Map;

    typedef typename Map::iterator Iterator;

    typedef typename Map::const_iterator ConstIterator;

    Map m_map;
};

template<typename FLOAT, typename INT>
void
SgStatisticsCollection<FLOAT,INT>
::Add(const SgStatisticsCollection<FLOAT,INT>& collection)
{
    if (m_map.size() != collection.m_map.size())
        throw SgException("Incompatible statistics collections");
    for (Iterator p = m_map.begin(); p != m_map.end(); ++p)
    {
        ConstIterator k = collection.m_map.find(p->first);
        if (k == collection.m_map.end())
            throw SgException("Incompatible statistics collections");
        p->second.Add(k->second);
    }
}

template<typename FLOAT, typename INT>
void SgStatisticsCollection<FLOAT,INT>::Clear()
{
    for (Iterator p = m_map.begin(); p != m_map.end(); ++p)
        p->second.Clear();
}

template<typename FLOAT, typename INT>
bool SgStatisticsCollection<FLOAT,INT>::Contains(const std::string& name)
    const
{
    return (m_map.find(name) != m_map.end());
}

template<typename FLOAT, typename INT>
void SgStatisticsCollection<FLOAT,INT>::Create(const std::string& name)
{
    m_map[name] = SgStatistics<FLOAT,INT>();
}

template<typename FLOAT, typename INT>
const SgStatistics<FLOAT,INT>&
SgStatisticsCollection<FLOAT,INT>::Get(const std::string& name) const
{
    ConstIterator p = m_map.find(name);
    if (p == m_map.end())
    {
        std::ostringstream o;
        o << "Unknown statistics name " << name << '.';
        throw SgException(o.str());
    }
    return p->second;
}

template<typename FLOAT, typename INT>
SgStatistics<FLOAT,INT>&
SgStatisticsCollection<FLOAT,INT>::Get(const std::string& name)
{
    Iterator p = m_map.find(name);
    if (p == m_map.end())
    {
        std::ostringstream o;
        o << "Unknown statistics name " << name << '.';
        throw SgException(o.str());
    }
    return p->second;
}

template<typename FLOAT, typename INT>
void SgStatisticsCollection<FLOAT,INT>::Write(std::ostream& o) const
{
    for (ConstIterator p = m_map.begin(); p != m_map.end(); ++p)
        o << p->first << ": " << p->second.Write(o) << '\n';
}

//----------------------------------------------------------------------------

/** Histogram.
    The template parameters are the floating point type and the counter type,
    depending on the precision-memory tradeoff.
*/
template<typename FLOAT, typename INT>
class SgHistogram
{
public:
    SgHistogram();

    SgHistogram(FLOAT min, FLOAT max, int bins);

    /** Reinitialize and clear histogram. */
    void Init(FLOAT min, FLOAT max, int bins);

    void Add(FLOAT value);

    void Clear();

    int Bins() const;

    INT Count() const;

    /** Get count in a certain bin. */
    INT Count(int i) const;

    /** Write as x,y-table.
        Writes the historgram in a format that likely can be used by other
        programs. Writes one x,y pair per line. The separator is TAB.
        The x-values are the left border values of the bins, the y-values
        are the counts of the bins.
    */
    void Write(std::ostream& out) const;

    /** Write with labels.
        Example output with label "Value", the numbers in brackets are the
        left border of each bin:
        @verbatim
        Value[0]  100
        Value[10] 2000
        Value[20] 500
        @endverbatim
    */
    void WriteWithLabels(std::ostream& out, const std::string& label) const;

private:
    typedef std::vector<INT> Vector;

    int m_bins;

    INT m_count;

    FLOAT m_binSize;

    FLOAT m_min;

    FLOAT m_max;

    Vector m_array;
};

template<typename FLOAT, typename INT>
SgHistogram<FLOAT,INT>::SgHistogram()
{
    Init(0, 1, 1);
}

template<typename FLOAT, typename INT>
SgHistogram<FLOAT,INT>::SgHistogram(FLOAT min, FLOAT max, int bins)
{
    Init(min, max, bins);
}

template<typename FLOAT, typename INT>
void SgHistogram<FLOAT,INT>::Add(FLOAT value)
{
    ++m_count;
    int i = static_cast<int>((value - m_min) / m_binSize);
    if (i < 0)
        i = 0;
    if (i >= m_bins)
        i = m_bins - 1;
    ++m_array[i];
}

template<typename FLOAT, typename INT>
int SgHistogram<FLOAT,INT>::Bins() const
{
    return m_bins;
}

template<typename FLOAT, typename INT>
void SgHistogram<FLOAT,INT>::Clear()
{
    m_count = 0;
    for (typename Vector::iterator it = m_array.begin(); it != m_array.end();
         ++ it)
        *it = 0;
}

template<typename FLOAT, typename INT>
INT SgHistogram<FLOAT,INT>::Count() const
{
    return m_count;
}

template<typename FLOAT, typename INT>
INT SgHistogram<FLOAT,INT>::Count(int i) const
{
    SG_ASSERT(i >= 0);
    SG_ASSERT(i < m_bins);
    return m_array[i];
}

template<typename FLOAT, typename INT>
void SgHistogram<FLOAT,INT>::Init(FLOAT min, FLOAT max, int bins)
{
    m_array.resize(bins);
    m_min = min;
    m_max = max;
    m_bins = bins;
    m_binSize = (m_max - m_min) / m_bins;
    Clear();
}

template<typename FLOAT, typename INT>
void SgHistogram<FLOAT,INT>::Write(std::ostream& out) const
{
    for (int i = 0; i < m_bins; ++i)
        out << (m_min + i * m_binSize) << '\t' << m_array[i] << '\n';

}

template<typename FLOAT, typename INT>
void SgHistogram<FLOAT,INT>::WriteWithLabels(std::ostream& out,
                                             const std::string& label) const
{
    for (int i = 0; i < m_bins; ++i)
    {
        std::ostringstream binLabel;
        binLabel << label << '[' << (m_min + i * m_binSize) << ']';
        out << SgWriteLabel(binLabel.str()) << m_array[i] << '\n';
    }
}

//----------------------------------------------------------------------------

#endif // SG_STATISTICS_H
