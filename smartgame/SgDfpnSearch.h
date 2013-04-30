//----------------------------------------------------------------------------
/** @file SgDfpnSearch.h */
//----------------------------------------------------------------------------

#ifndef SG_DFPN_SEARCH_H
#define SG_DFPN_SEARCH_H

#include "SgBoardColor.h"
#include "SgHashTable.h"
#include "SgStatistics.h"
#include "SgTimer.h"
#include "SgSearchTracer.h"

#include <limits>
#include <ostream>
#include <boost/scoped_ptr.hpp>

typedef std::vector<SgMove> PointSequence;

//----------------------------------------------------------------------------

/** @defgroup dfpn Depth-First Proof Number Search
    
    Based on Jakub Pawlewicz' code for df-pn in Hex.
    Also implements focused dfpn (Hayward et al).
*/

//----------------------------------------------------------------------------

/** Statistics tracker used in dfpn search.
    @ingroup dfpn
*/
typedef SgStatisticsExt<float, std::size_t> DfpnStatistics;

//----------------------------------------------------------------------------

/** Type used for bounds. 
    @ingroup dfpn
*/
typedef unsigned DfpnBoundType;

/** Bounds used in Dfpn search. 
    @ingroup dfpn
*/
struct DfpnBounds
{
    /** Denotes a proven state. */
    static const DfpnBoundType INFTY = 2000000000;

    /** Maximum amount of work. Must be less than INFTY. */ 
    static const DfpnBoundType MAX_WORK = INFTY / 2;

    /** Proof number.
        Estimated amount of work to prove this state winning. */
    DfpnBoundType phi;

    /** Disproof number.
        Estimated amount of work to prove this state losing. */
    DfpnBoundType delta;

    DfpnBounds();

    DfpnBounds(DfpnBoundType p, DfpnBoundType d);


    /** Returns true if phi is greater than other's phi and delta is
        greater than other's delta. */
    bool GreaterThan(const DfpnBounds& other) const;

    /** Returns true if bounds are winning (phi is 0). */
    bool IsWinning() const;

    /** Returns true if bounds are losing (delta is 0). */
    bool IsLosing() const;
    
    /** Returns true if IsWinning() or IsLosing() is true. */
    bool IsSolved() const;

    void CheckConsistency() const;

    /** Print bounds in human readable format. */
    std::string Print() const;

    /** Sets the bounds to (0, INFTY). */
    static void SetToWinning(DfpnBounds& bounds);

    /** Sets the bounds to (INFTY, 0). */
    static void SetToLosing(DfpnBounds& bounds);
};

inline DfpnBounds::DfpnBounds()
    : phi(INFTY), 
      delta(INFTY)
{
}

inline DfpnBounds::DfpnBounds(DfpnBoundType p, DfpnBoundType d)
    : phi(p), 
      delta(d)
{
}

inline std::string DfpnBounds::Print() const
{
    std::ostringstream os;
    os << "[" << phi << ", " << delta << "]";
    return os.str();
}

inline bool DfpnBounds::GreaterThan(const DfpnBounds& other) const
{
    return (phi > other.phi) && (delta > other.delta);
}

inline bool DfpnBounds::IsWinning() const
{
    return phi == 0;
}

inline bool DfpnBounds::IsLosing() const
{
    return delta == 0;
}

inline bool DfpnBounds::IsSolved() const
{
    return IsWinning() || IsLosing();
}

inline void DfpnBounds::SetToWinning(DfpnBounds& bounds)
{
    bounds.phi = 0;
    bounds.delta = INFTY;
}

inline void DfpnBounds::SetToLosing(DfpnBounds& bounds)
{
    bounds.phi = INFTY;
    bounds.delta = 0;
}

/** Extends global output operator for DfpnBounds. */
inline std::ostream& operator<<(std::ostream& os, const DfpnBounds& bounds)
{
    os << bounds.Print();
    return os;
}

//----------------------------------------------------------------------------

/** Children of a dfpn state. 
    @ingroup dfpn
*/
class DfpnChildren
{
public:
    DfpnChildren();

    const std::vector<SgMove>& Children() const;
    std::vector<SgMove>& Children();
    
    std::size_t Size() const;

    SgMove MoveAt(std::size_t index) const;

private:
    friend class DfpnSolver;

    std::vector<SgMove> m_children;
};

inline std::size_t DfpnChildren::Size() const
{
    return m_children.size();
}

inline SgMove DfpnChildren::MoveAt(std::size_t index) const
{
    return m_children[index];
}

inline const std::vector<SgMove>& DfpnChildren::Children() const
{
    return m_children;
}

inline std::vector<SgMove>& DfpnChildren::Children()
{
    return m_children;
}

//----------------------------------------------------------------------------

/** State in DfpnHashTable.
    @ingroup dfpn
 */
class DfpnData
{
public:
    DfpnBounds m_bounds;

    SgMove m_bestMove;
    
    size_t m_work;

    DfpnData();

    DfpnData(const DfpnBounds& bounds, SgMove bestMove, size_t work);

    ~DfpnData();

    std::string Print() const; 
    
    /** @name SgHashTable methods. */
    // @{

    bool IsValid() const;

    void Invalidate();
    
    bool IsBetterThan(const DfpnData& data) const;

    // @}

private:
    bool m_isValid;
};


inline DfpnData::DfpnData()
    : m_isValid(false)
{ }

inline DfpnData::DfpnData(const DfpnBounds& bounds, 
                          SgMove bestMove, size_t work)
    : m_bounds(bounds),
      m_bestMove(bestMove),
      m_work(work),
      m_isValid(true)
{  }

inline DfpnData::~DfpnData()
{ }

inline std::string DfpnData::Print() const
{
    std::ostringstream os;
    os << '[' 
       << "bounds=" << m_bounds << ' '
       << "bestmove=" << m_bestMove << ' '
       << "work=" << m_work << ' '
       << ']';
    return os.str();
}

inline bool DfpnData::IsBetterThan(const DfpnData& data) const
{
    return m_work > data.m_work;
}

inline bool DfpnData::IsValid() const
{
    return m_isValid;
}

inline void DfpnData::Invalidate()
{
    m_isValid = false;
}

/** Extends global output operator for DfpnData. */
inline std::ostream& operator<<(std::ostream& os, const DfpnData& data)
{
    os << data.Print();
    return os;
}

//----------------------------------------------------------------------------

/** History of moves played from root state to current state. 
    @ingroup dfpn
*/
class DfpnHistory
{
public:
    DfpnHistory();

    /** Adds a new state to the history. */
    void Push(SgMove m_move, SgHashCode hash);

    /** Removes last stated added from history. */
    void Pop();

    /** Returns number of moves played so far. */
    int Depth() const;

    /** Hash of last state. */
    SgHashCode LastHash() const;

    /** Move played from parent state to bring us to this state. */
    SgMove LastMove() const;

private:

    /** Move played from state. */
    std::vector<SgMove> m_move;

    /** Hash of state. */
    std::vector<SgHashCode> m_hash;
};

inline DfpnHistory::DfpnHistory()
{
    m_move.push_back(SG_NULLMOVE);
    m_hash.push_back(0);
}

inline void DfpnHistory::Push(SgMove move, SgHashCode hash)
{
    m_move.push_back(move);
    m_hash.push_back(hash);
}

inline void DfpnHistory::Pop()
{
    m_move.pop_back();
    m_hash.pop_back();
}

inline int DfpnHistory::Depth() const
{
    SG_ASSERT(! m_move.empty());
    return static_cast<int>(m_move.size() - 1);
}

inline SgHashCode DfpnHistory::LastHash() const
{
    return m_hash.back();
}

inline SgMove DfpnHistory::LastMove() const
{
    return m_move.back();
}

//----------------------------------------------------------------------------

/** Hashtable used in dfpn search.  
    @ingroup dfpn
*/
typedef SgHashTable<DfpnData, 4> DfpnHashTable;

//----------------------------------------------------------------------------

/** Solver using DFPN search. 
    @ingroup dfpn
*/
class DfpnSolver 
{
public:

    DfpnSolver();

    virtual ~DfpnSolver();

    /** Solve the given state using the given hashtable. 
        Returns the color of the winning player (SG_EMPTY if it could
        not determine a winner in time). */
    SgEmptyBlackWhite
    StartSearch(DfpnHashTable& positions, PointSequence& pv);

    SgEmptyBlackWhite 
    StartSearch(DfpnHashTable& positions, PointSequence& pv,
                         const DfpnBounds& maxBounds);

    /** Validate the current position is a win for winner. */
    bool Validate(DfpnHashTable& positions, const SgBlackWhite winner,
                  SgSearchTracer& tracer);

    /** Returns various histograms pertaining to the evaluation
        function from the last search. */
    std::string EvaluationInfo() const;

    /** Generate moves for children */
    virtual void GenerateChildren(std::vector<SgMove>& children) const = 0;

    /** Execute move, must be legal. */
    virtual void PlayMove(SgMove move) = 0;

    /** Undo effects of most recent PlayMove, take back move */
    virtual void UndoMove() = 0;
    
    /** Set winner if it is terminal */
    virtual bool TerminalState(SgBoardColor colorToPlay, 
                               SgEmptyBlackWhite& winner)  = 0; //const
    
    /** For sorting children. Default implementation returns constant */
    virtual float Score(SgMove move) const; // 
    
    /** ToPlay */
    virtual SgBoardColor GetColorToMove() const = 0;

    /** Hashcode of current position */
    virtual SgHashCode Hash() const = 0;
    
    /** Convert SgMove's in sequence to game-specific strings */
    virtual void WriteMoveSequence(std::ostream& stream,
                                   const PointSequence& sequence) const = 0;

    size_t NumGenerateMovesCalls() const;
    
    size_t NumMIDcalls() const;
    
    size_t NumTerminalNodes() const;

    //------------------------------------------------------------------------

    /** @name Parameters */
    // @{

    /** Maximum time search is allowed to run before aborting. 
        Set to 0 for no timelimit. */
    double Timelimit() const;

    /** See Timelimit() */
    void SetTimelimit(double timelimit);

    /** Widening base affects what number of the moves to consider
        are always looked at by the dfpn search (omitting losing moves),
        regardless of branching factor. This amount is added to the
        proportion computed by the WideningFactor (see below).
        The base must be set to at least 1. */
    int WideningBase() const;

    /** See WideningBase() */
    void SetWideningBase(int wideningBase);
    
    /** Widening factor affects what fraction of the moves to consider
        are looked at by the dfpn search (omitting losing moves).
        Must be in the range (0, 1], where 1 ensures no pruning. */
    float WideningFactor() const;

    /** See WideningFactor() */
    void SetWideningFactor(float wideningFactor);

    /** Epsilon is the epsilon used in 1+epsilon trick,
     *  i.e. when setting bounds for a child MID call
     *  delta2 * (1+epsilon) is used instead delta2 + 1 */
    float Epsilon() const;
    
    /** See Epsilon() */
    void SetEpsilon(float epsilon);
    
    // @}

private:

    DfpnHashTable* m_hashTable;

    SgTimer m_timer;

    /** See TimeLimit() */
    double m_timelimit;

    /** See WideningBase() */
    int m_wideningBase;

    /** See WideningFactor() */
    float m_wideningFactor;

    /** See Epsilon() */
    float m_epsilon;

    /** Number of calls to CheckAbort() before we check the timer.
        This is to avoid expensive calls to SgTime::Get(). Try to scale
        this so that it is checked twice a second. */
    size_t m_checkTimerAbortCalls;

    bool m_aborted;

    size_t m_numTerminal;

    size_t m_numMIDcalls;

    size_t m_generateMoves;

    SgStatisticsExt<float, std::size_t> m_prunedSiblingStats;

    SgStatisticsExt<float, std::size_t> m_moveOrderingPercent;

    SgStatisticsExt<float, std::size_t> m_moveOrderingIndex;

    SgStatisticsExt<float, std::size_t> m_deltaIncrease;

    size_t m_totalWastedWork;

    size_t MID(const DfpnBounds& n, DfpnHistory& history);

    void SelectChild(std::size_t& bestIndex, DfpnBoundType& delta2, 
                     const std::vector<DfpnData>& childrenDfpnBounds,
                     size_t maxChildIndex) const;

    void UpdateBounds(DfpnBounds& bounds, 
                      const std::vector<DfpnData>& childBounds,
                      size_t maxChildIndex) const;

    bool CheckAbort();

    void LookupData(DfpnData& data, const DfpnChildren& children, 
                    std::size_t childIndex);

    virtual bool TTRead(DfpnData& data);

    virtual void TTWrite(const DfpnData& data);

    void PrintStatistics(SgEmptyBlackWhite winner, const PointSequence& p) const;

    // reconstruct the pv by following the best moves in hash table.
    // todo make const, use ModBoard. (game-specific)
    void GetPVFromHash(PointSequence& pv);
    
    size_t ComputeMaxChildIndex(const std::vector<DfpnData>&
                                childrenData) const;
};

inline float DfpnSolver::Epsilon() const
{
    return m_epsilon;
}

inline size_t DfpnSolver::NumGenerateMovesCalls() const
{
    return m_generateMoves;
}

inline size_t DfpnSolver::NumMIDcalls() const
{
    return m_numMIDcalls;
}

inline size_t DfpnSolver::NumTerminalNodes() const
{
    return m_numTerminal;
}

inline float DfpnSolver::Score(SgMove move) const
{
	SG_UNUSED(move);
    return 1.0f; // override
}

inline void DfpnSolver::SetEpsilon(float epsilon)
{
    m_epsilon = epsilon;
}

inline void DfpnSolver::SetTimelimit(double timelimit)
{
    m_timelimit = timelimit;
}

inline void DfpnSolver::SetWideningBase(int wideningBase)
{
    m_wideningBase = wideningBase;
}

inline void DfpnSolver::SetWideningFactor(float wideningFactor)
{
    m_wideningFactor = wideningFactor;
}

inline double DfpnSolver::Timelimit() const
{
    return m_timelimit;
}

inline bool DfpnSolver::TTRead(DfpnData& data)
{
    return m_hashTable->Lookup(Hash(), &data);
}

inline void DfpnSolver::TTWrite(const DfpnData& data)
{
    #ifndef NDEBUG
        data.m_bounds.CheckConsistency();
    #endif
    m_hashTable->Store(Hash(), data);
}

inline int DfpnSolver::WideningBase() const
{
    return m_wideningBase;
}

inline float DfpnSolver::WideningFactor() const
{
    return m_wideningFactor;
}

//----------------------------------------------------------------------------

#endif // SG_DFPN_SEARCH_H
