//----------------------------------------------------------------------------
/** @file FePattern.cpp  */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FePattern.h"

#include "FePatternBase.h"
#include "GoSetupUtil.h"
#include "SgWrite.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

FePattern::FePattern(size_t nuPoints)
  : m_nuPoints(nuPoints),
    m_move(SG_NULLPOINT),
    m_color(SG_BLACK),
    m_value(0.0)
{ }

FePattern::~FePattern()
{ }

/** Initialize pattern points from string. */
void FePattern::Init(const std::string& s)
{
    SG_DEBUG_ONLY(s);
    SG_ASSERT(s.size() >= m_nuPoints);
    int boardSize = 0;
    GoSetup setup =
        GoSetupUtil::CreateSetupFromString(s, boardSize);
    SG_ASSERT(boardSize > 0);
    
    int width = boardSize;
    int height = boardSize; // @todo rectangles
    
    for (int i = 1; i <= width; ++i)
        for (int j = 1; j <= height; ++j)
        {
            SgPoint p = Pt(i,j);
            SgBoardColor color;
            if (setup.m_stones[SG_BLACK].Contains(p))
                color = SG_BLACK;
            else if (setup.m_stones[SG_WHITE].Contains(p))
                color = SG_WHITE;
//@todo            if (setup.m_offboard.Contains(p))
//                color = SG_BORDER;
            else
                color = SG_EMPTY;
            
            // @todo use offset or just p for index???
            // int offset = PaConst::Offset(int deltaX, int deltaY)
            if (color != SG_BORDER)
            {
                PaPoint pa(p, color);
                m_points.push_back(pa);
            }
       }
}

void FePattern::SetMove(SgPoint p, SgBlackWhite color)
{
    m_move = p;
    m_color = color;
}

void FePattern::SetValue(float value)
{
    m_value = value;
}

inline bool CompareMove(const FePattern::PaPoint& p1,
                        const FePattern::PaPoint& p2)
{
    return p1.first < p2.first;
}

SgRect EnclosingRect(const std::vector<FePattern::PaPoint>& points)
{
    SgRect r;
    for (std::vector<FePattern::PaPoint>::const_iterator it
         = points.begin(); it != points.end(); ++it)
        r.Include(it->first);
    return r;
}

void WritePatternMap(std::ostream& stream,
                     const std::vector<FePattern::PaPoint>& origPoints)
{
    std::vector<FePattern::PaPoint> points(origPoints);
    std::sort(points.begin(), points.end(), CompareMove);
    SgRect enclosingRect = EnclosingRect(points);
    std::vector<FePattern::PaPoint>::const_iterator pit = points.begin();
    for (SgRectIterator it(enclosingRect); it; ++it)
    {
        if (pit == points.end() || (*it < pit->first))
            stream << ' ';
        else
        {
            SG_ASSERT(*it == pit->first);
            stream << SgEBW(pit->second); // color of point
            ++pit; // advance to next point in pattern
        }
        if (it.AtEndOfLine())
            stream << '\n';
    }
    SG_ASSERT(pit == points.end());
}

void FePattern::Write(std::ostream& stream) const
{
    stream << "FePattern nuPoints " << NuPoints()
    << ", move " << SgWriteMove(Move(), Color())
    << ", value " << Value() << '\n';
    WritePatternMap(stream, m_points);
}

std::ostream& operator<<(std::ostream& stream, const FePattern& p)
{
    p.Write(stream);
    return stream;
}

//----------------------------------------------------------------------------

FeRectPattern::FeRectPattern(size_t width, size_t height)
  : FePattern(width * height),
    m_width(width),
    m_height(height)
{ }

FeRectPattern::~FeRectPattern()
{ }

void FeRectPattern::Write(std::ostream& stream) const
{
    stream << "FeRectPattern [" << Width() << 'x' << Height() << "]\n";
    FePattern::Write(stream);
}

//----------------------------------------------------------------------------

void FePattern::SetPoints(const GoBoard& bd,
                          const std::vector<SgPoint>& points)
{
    for (std::vector<SgPoint>::const_iterator it
         = points.begin(); it != points.end(); ++it)
    {
        SgEmptyBlackWhite color = bd.GetColor(*it);
        FePattern::PaPoint pa(*it, color);
        m_points.push_back(pa);
    }
}

FeRectPattern* DefineRectPattern(const GoBoard& bd,
                                 PaSpot spot,
                                 int width, int height)
{
    FeRectPattern* pat = new FeRectPattern(width, height);
    std::vector<SgPoint> points;
    for (int w = 0; w < width; ++w)
        for (int h = 0; h < height; ++h)
        {
            SgPoint p = PatternToPoint(w, h, spot, bd.Size());
            SG_ASSERT(bd.IsValidPoint(p));
            points.push_back(p);
        }
    pat->SetPoints(bd, points);
    return pat;
}

//----------------------------------------------------------------------------

FePatternMatch::FePatternMatch(SgPoint move, SgBlackWhite color, float value)
  : m_move(move),
    m_color(color),
    m_value(value)
{ }

//----------------------------------------------------------------------------

FePatternMatchResult::FePatternMatchResult()
{ }
    
size_t FePatternMatchResult::Length() const
{
    return m_match.size();
}

void FePatternMatchResult::AddMatch(const FePatternMatch& match)
{
    m_match.push_back(match);
}

const FePatternMatch& FePatternMatchResult::Match(size_t index) const
{
    SG_ASSERT(index < Length());
    return m_match[index];
}

//----------------------------------------------------------------------------

FePatternMatcher::FePatternMatcher()
{ }

FePatternMatcher::~FePatternMatcher()
{ }

FePatternMatchResult* FePatternMatcher::Match(const GoBoard& bd) const
{
    SG_UNUSED(bd);
    FePatternMatch pm(Pt(3,3), SG_BLACK, 23);
    FePatternMatchResult* r = new FePatternMatchResult();
    r->AddMatch(pm);
    return r;
}
    
void FePatternMatcher::Add(const FePattern* pattern)
{
    m_patterns.push_back(pattern);
}

//----------------------------------------------------------------------------

FeRectPatternMatcher::FeRectPatternMatcher()
  : FePatternMatcher()
{ }

FeRectPatternMatcher::~FeRectPatternMatcher()
{ }

void FeRectPatternMatcher::Add(const FePattern* pattern)
{
    FePatternMatcher::Add(pattern);
    // TODO
}
