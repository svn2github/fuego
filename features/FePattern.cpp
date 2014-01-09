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

std::ostream& operator<<(std::ostream& stream, const FePattern& p)
{
    stream << "FePattern nuPoints " << p.NuPoints()
    << ", move " << SgWriteMove(p.Move(), p.Color())
    << ", value " << p.Value() << '\n';
    // @todo m_points
    
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

std::ostream& operator<<(std::ostream& stream, const FeRectPattern& r)
{
    stream << "FeRectPattern [" << r.Width() << 'x' << r.Height() << "]\n";
    // call inherited. FePattern::Write(stream);
    return stream;
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
