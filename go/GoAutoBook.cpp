//----------------------------------------------------------------------------
/** @file GoAutoBook.cpp 
 */
//----------------------------------------------------------------------------

#include "GoAutoBook.h"

//----------------------------------------------------------------------------

GoAutoBookState::GoAutoBookState(const GoBoard& brd)
    : m_synchronizer(brd)
{
    m_synchronizer.SetSubscriber(m_brd[0]);
}

GoAutoBookState::~GoAutoBookState()
{
}

SgHashCode GoAutoBookState::GetHashCode() const
{
    return m_hash;
}

void GoAutoBookState::Synchronize()
{
    m_synchronizer.UpdateSubscriber();
    int size = m_brd[0].Size();
    int numMoves = m_brd[0].MoveNumber();
    for (int rot = 1; rot < 8; ++rot)
    {
        m_brd[rot].Init(size, size);
        for (int i = 0; i < numMoves; ++i)
        {
            SgMove move = m_brd[0].Move(i).Point();
            m_brd[rot].Play(SgPointUtil::Rotate(rot, move, size));
        }
    }
    ComputeHashCode();
}

void GoAutoBookState::Play(SgMove move)
{
    m_hash = m_brd[0].GetHashCodeInclToPlay();
    for (int rot = 0; rot < 8; ++rot)
        m_brd[rot].Play(SgPointUtil::Rotate(rot, move, m_brd[0].Size()));
    ComputeHashCode();
}

void GoAutoBookState::Undo()
{
    for (int rot = 0; rot < 8; ++rot)
        m_brd[rot].Undo();
    ComputeHashCode();
}

void GoAutoBookState::ComputeHashCode()
{
    m_hash = m_brd[0].GetHashCodeInclToPlay();
    for (int rot = 1; rot < 8; ++rot)
    {
        SgHashCode curHash = m_brd[rot].GetHashCodeInclToPlay();
        if (curHash < m_hash)
            m_hash = curHash;
    }
}

//----------------------------------------------------------------------------

GoAutoBook::GoAutoBook(const std::string& filename) throw()
    : m_filename(filename)
{
    std::ifstream is(filename.c_str());
    if (!is)
    {
        std::ofstream of(filename.c_str());
        if (!of)
            throw SgException("Invalid file name!");
        of.close();
    }
    else
    {
        while (is)
        {
            std::string line;
            std::getline(is, line);
            if (line.size() < 19)
                continue;
            std::string str;
            std::istringstream iss(line);
            iss >> str;
            SgHashCode hash;
            hash.FromString(str);
            SgBookNode node(line.substr(19));
            m_data[hash] = node;
        }
        SgDebug() << "GoAutoBook: Parsed " << m_data.size() << " lines.\n";
    }
}

GoAutoBook::~GoAutoBook()
{
}

bool GoAutoBook::Get(const GoAutoBookState& state, SgBookNode& node)
{
    Map::iterator it = m_data.find(state.GetHashCode());
    if (it != m_data.end())
    {
        node = it->second;
        return true;
    }
    return false;
}

void GoAutoBook::Put(const GoAutoBookState& state, const SgBookNode& node)
{
    m_data[state.GetHashCode()] = node;
}

void GoAutoBook::Flush()
{
    Save(m_filename);
}

void GoAutoBook::Save(const std::string& filename) const
{
    std::ofstream out(filename.c_str());
    for (Map::const_iterator it = m_data.begin(); it != m_data.end(); ++it)
    {
        out << it->first.ToString() << '\t' 
            << it->second.ToString() << '\n';
    }
    out.close();
}

//----------------------------------------------------------------------------
