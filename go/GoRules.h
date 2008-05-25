//----------------------------------------------------------------------------
/** @file GoRules.h
    Class GoRules.
*/
//----------------------------------------------------------------------------

#ifndef GORULES_H
#define GORULES_H

#include <iosfwd>
#include <string>
#include "GoKomi.h"

//----------------------------------------------------------------------------

/** Parameters describing game rules and handicap. */
class GoRules
{
public:
    /** Ko rule. */
    enum KoRule
    {
        /** Positional superko.
            Full board repetition is forbidden, independent on who is to play.
        */
        POS_SUPERKO,

        /** Only repetition of the position two moves ago is forbidden. */
        SIMPLEKO,

        /** Situational superko.
            Full board repetition is forbidden, including who is to play.
        */
        SUPERKO
    };

    GoRules(int handicap = 0, const GoKomi& komi = GoKomi(6.5),
            bool japanese = false, bool twoPassesEndGame = true);

    bool operator==(const GoRules& rules) const;

    bool operator!=(const GoRules& rules) const;

    /** Default is false. */
    bool AllowSuicide() const;

    /** Whether it necessary to capture dead stones.
        With some rules all un-captured stones count as alive.
        Default is false.
    */
    bool CaptureDead() const;

    KoRule GetKoRule() const;

    int Handicap() const;

    /** True if using Japanese style handicap. */
    bool JapaneseHandicap() const;
    
    /** True if using Japanese style scoring.
        Japanese style scoring counts territory and prisoners, but not
        own stones.
    */
    bool JapaneseScoring() const;
    
    const GoKomi& Komi() const;

    void SetAllowSuicide(bool allowSuicide);

    /** See CaptureDead() */
    void SetCaptureDead(bool captureDead);

    void SetHandicap(int handicap);

    void SetJapaneseHandicap(bool japaneseHandicap);

    void SetJapaneseScoring(bool japaneseScoring);

    void SetKomi(const GoKomi& komi);

    void SetKoRule(KoRule koRule);

    /** Set several rule settings according to rule name.
        Currently supported:
        <table>
        <tr>
        <th>Name</th>
        <th>Suicide</th>
        <th>JapaneseHandicap</th>
        <th>JapaneseScoring</th>
        <th>KoRule</th>
        <th>CaptureDead</th>
        </tr>
        <tr>
        <td>cgos</td>
        <td>no</td>
        <td>no</td>
        <td>no</td>
        <td>positional superko</td>
        <td>yes</td>
        </tr>
        <tr>
        <td>chinese</td>
        <td>no</td>
        <td>no</td>
        <td>no</td>
        <td>superko</td>
        <td>no</td>
        </tr>
        <tr>
        <td>japanese</td>
        <td>no</td>
        <td>yes</td>
        <td>yes</td>
        <td>simple</td>
        <td>no</td>
        </tr>
        <tr>
        <td>kgs</td>
        <td>no</td>
        <td>no</td>
        <td>no</td>
        <td>positional superko</td>
        <td>no</td>
        </tr>
        </table>
        @param namedRules The named rules.
        @exception SgException If rule name is not known.
     */
    void SetNamedRules(const std::string& namedRules);

    void SetTwoPassesEndGame(bool twoPassesEndGame);

    /** True if two passes end the game, false if 3 passes needed. */
    bool TwoPassesEndGame() const;

private:
    bool m_allowSuicide;

    bool m_captureDead;

    bool m_japaneseScoring;

    /** Initial handicap for this game. */
    int m_handicap;

    /** The komi. */
    GoKomi m_komi;

    bool m_japaneseHandicap;

    bool m_twoPassesEndGame;

    KoRule m_koRule;
};

inline bool GoRules::operator!=(const GoRules& rules) const
{
    return ! (*this == rules);
}

inline bool GoRules::AllowSuicide() const
{
    return m_allowSuicide;
}

inline bool GoRules::CaptureDead() const
{
    return m_captureDead;
}

inline GoRules::KoRule GoRules::GetKoRule() const
{
    return m_koRule;
}

inline int GoRules::Handicap() const
{
    return m_handicap;
}

inline bool GoRules::JapaneseHandicap() const
{
    return m_japaneseHandicap;
}
    
inline bool GoRules::JapaneseScoring() const
{
    return m_japaneseScoring;
}

inline const GoKomi& GoRules::Komi() const
{
    return m_komi;
}

inline void GoRules::SetAllowSuicide(bool allowSuicide)
{
    m_allowSuicide = allowSuicide;
}

inline void GoRules::SetCaptureDead(bool captureDead)
{
    m_captureDead = captureDead;
}

inline void GoRules::SetHandicap(int handicap)
{
    SG_ASSERT(handicap >= 0);
    m_handicap = handicap;
}

inline void GoRules::SetJapaneseHandicap(bool japaneseHandicap)
{
    m_japaneseHandicap = japaneseHandicap;
}

inline void GoRules::SetJapaneseScoring(bool japaneseScoring)
{
    m_japaneseScoring = japaneseScoring;
}

inline void GoRules::SetKomi(const GoKomi& komi)
{
    m_komi = komi;
}

inline void GoRules::SetKoRule(KoRule koRule)
{
    m_koRule = koRule;
}

inline void GoRules::SetTwoPassesEndGame(bool twoPassesEndGame)
{
    m_twoPassesEndGame = twoPassesEndGame;
}

inline bool GoRules::TwoPassesEndGame() const
{
    return m_twoPassesEndGame;
}

//----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, GoRules::KoRule koRule);

//----------------------------------------------------------------------------

#endif // GORULES_H

