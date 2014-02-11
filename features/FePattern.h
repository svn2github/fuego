//----------------------------------------------------------------------------
/** @file FePattern.h */
//----------------------------------------------------------------------------

#ifndef FEPATTERN_H
#define FEPATTERN_H

#include <iostream>
#include <string>
#include "FePatternBase.h"
#include "GoBoard.h"

//----------------------------------------------------------------------------

class FePattern
{
public:
    FePattern(size_t nuPoints);
    
    virtual ~FePattern();

    /** Initialize pattern points from string. */
    void Init(const std::string& s);

    void SetMove(SgPoint p, SgBlackWhite color);
    
    void SetPoints(const GoBoard& bd, const std::vector<SgPoint>& points);

    void SetValue(float value);

    size_t NuPoints() const {return m_nuPoints;}

    SgPoint Move() const {return m_move;}
    
    SgBlackWhite Color() const {return m_color;}
    
    float Value() const {return m_value;}

    virtual void Write(std::ostream& stream) const;
    
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

    void Write(std::ostream& stream) const;

private:
    size_t m_width;
    
    size_t m_height;
};

//----------------------------------------------------------------------------

FeRectPattern* DefineRectPattern(const GoBoard& bd, PaSpot spot,
                                 int width, int height);

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
    virtual ~FePatternMatcher();

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
    virtual ~FeRectPatternMatcher();

    void Add(const FePattern* pattern);

private:
};

//----------------------------------------------------------------------------

#endif // FEPATTERN_H
