//----------------------------------------------------------------------------
/** @file FePattern.h */
//----------------------------------------------------------------------------

#ifndef FEPATTERN_H
#define FEPATTERN_H

#include <iostream>
#include <string>
#include "GoBoard.h"

//----------------------------------------------------------------------------

class FePattern
{
public:
    FePattern(size_t nuPoints);
    
    ~FePattern();

    /** Initialize pattern points from string. */
    void Init(const std::string& s);

    void SetMove(SgPoint p, SgBlackWhite color);
    
    void SetValue(float value);

    size_t NuPoints() const {return m_nuPoints;}

    SgPoint Move() const {return m_move;}
    
    SgBlackWhite Color() const {return m_color;}
    
    float Value() const {return m_value;}
    
    typedef std::pair<int,SgBoardColor> PaPoint;
    
private:
    std::vector<PaPoint> m_points;
    
    size_t m_nuPoints;
    
    SgPoint m_move;
    
    SgBlackWhite m_color;
    
    float m_value;
};

std::ostream& operator<<(std::ostream& stream, const FePattern& p);


//----------------------------------------------------------------------------

class FeRectPattern : public FePattern
{
public:
    FeRectPattern(size_t width, size_t height);
    
    ~FeRectPattern();

    size_t Width() const {return m_width;}
    
    size_t Height() const {return m_height;}

private:
    size_t m_width;
    
    size_t m_height;
};

std::ostream& operator<<(std::ostream& stream, const FeRectPattern& r);

//----------------------------------------------------------------------------

class FePatternMatch
{
public:
    FePatternMatch(SgPoint move, SgBlackWhite color, float value);
    
    SgPoint Move() const {return m_move;}
    
    SgPoint MoveColor() const {return m_color;}
    
    float Value() const {return m_value;}
private:
    SgPoint m_move;
    
    SgBlackWhite m_color;
    
    float m_value;
};

//----------------------------------------------------------------------------

class FePatternMatchResult
{
public:
    FePatternMatchResult();
    
    size_t Length() const;
    
    void AddMatch(const FePatternMatch& match);
    
    const FePatternMatch& Match(size_t index) const;
    
private:
    std::vector<FePatternMatch> m_match;
};

//----------------------------------------------------------------------------
class FePatternMatcher
{
public:
    FePatternMatcher();
    
    /** Creates a new FePatternMatchResult and returns it. */
    FePatternMatchResult* Match(const GoBoard& bd) const;
    
    virtual void Add(const FePattern* pattern);
    
private:
    std::vector<const FePattern*> m_patterns;
};

//----------------------------------------------------------------------------
class FeRectPatternMatcher : public FePatternMatcher
{
public:
    FeRectPatternMatcher();
    
private:
};

//----------------------------------------------------------------------------

#endif // FEPATTERN_H
