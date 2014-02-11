//----------------------------------------------------------------------------
/** @file FeBasicFeatures.h
    Non-patterns features as in Remi Coulom's work.
*/
//----------------------------------------------------------------------------

#ifndef FE_BASIC_FEATURES_H
#define FE_BASIC_FEATURES_H

//----------------------------------------------------------------------------

#include <bitset>
#include <iosfwd>
#include "GoLadder.h"
#include "SgPointArray.h"

//----------------------------------------------------------------------------

// Coulom's classical features
enum FeBasicFeature{
    FE_PASS_NEW,            // pass, previous move was not pass
    FE_PASS_CONSECUTIVE,    // pass, previous move was also pass
    FE_CAPTURE_ADJ_ATARI,   // String contiguous to new string in atari
    FE_CAPTURE_RECAPTURE,   // Re-capture previous move
    FE_CAPTURE_PREVENT_CONNECTION, // Prevent connection to previous move
    FE_CAPTURE_NOT_LADDER,  // String not in a ladder
    FE_CAPTURE_LADDER,      // String in a ladder
    // FE_CAPTURE_MULTIPLE,
    FE_EXTENSION_NOT_LADDER, // New atari, not in a ladder
    FE_EXTENSION_LADDER,    // New atari, in a ladder
    // todo distinguish extending 1 stone only?
    FE_SELFATARI,
    // FE_SELFATARI_NAKADE,
    // FE_SELFATARI_THROWIN,
    FE_ATARI_LADDER,        // Ladder atari
    FE_ATARI_KO,            // Atari when there is a ko
    FE_ATARI_OTHER,         // Other atari
    FE_LINE_1,
    FE_LINE_2,
    FE_LINE_3,
    FE_LINE_4,
    FE_LINE_5_OR_MORE,
    FE_DIST_PREV_2, // d(dx,dy) = |dx|+|dy|+max(|dx|,|dy|)
    FE_DIST_PREV_3,
    FE_DIST_PREV_4,
    FE_DIST_PREV_5,
    FE_DIST_PREV_6,
    FE_DIST_PREV_7,
    FE_DIST_PREV_8,
    FE_DIST_PREV_9,
    FE_DIST_PREV_10,
    FE_DIST_PREV_11,
    FE_DIST_PREV_12,
    FE_DIST_PREV_13,
    FE_DIST_PREV_14,
    FE_DIST_PREV_15,
    FE_DIST_PREV_16,
    FE_DIST_PREV_17,
    FE_DIST_PREV_OWN_0,
    FE_DIST_PREV_OWN_2,
    FE_DIST_PREV_OWN_3,
    FE_DIST_PREV_OWN_4,
    FE_DIST_PREV_OWN_5,
    FE_DIST_PREV_OWN_6,
    FE_DIST_PREV_OWN_7,
    FE_DIST_PREV_OWN_8,
    FE_DIST_PREV_OWN_9,
    FE_DIST_PREV_OWN_10,
    FE_DIST_PREV_OWN_11,
    FE_DIST_PREV_OWN_12,
    FE_DIST_PREV_OWN_13,
    FE_DIST_PREV_OWN_14,
    FE_DIST_PREV_OWN_15,
    FE_DIST_PREV_OWN_16,
    FE_DIST_PREV_OWN_17,
    FE_GOUCT_FILLBOARD, // FE_GOUCT features correspond to Fuego policy
    FE_GOUCT_NAKADE,    // functions as in GoUctPlayoutPolicy.h
    FE_GOUCT_ATARI_CAPTURE,
    FE_GOUCT_ATARI_DEFEND,
    FE_GOUCT_LOWLIB,
    FE_GOUCT_PATTERN,
    FE_GOUCT_CAPTURE,
    FE_GOUCT_RANDOM_PRUNED, // not generated as a random move
    FE_GOUCT_REPLACE_CAPTURE_FROM, // this move gets moved elsewhere
    FE_GOUCT_REPLACE_CAPTURE_TO,   // some other move gets moved here
    FE_GOUCT_REPLACE_CAPTURE_NOT_MOVED, // not changed by correction
    FE_GOUCT_SELFATARI_CORRECTION_FROM, // this move gets moved elsewhere
    FE_GOUCT_SELFATARI_CORRECTION_TO,   // some other move gets moved here
    FE_GOUCT_SELFATARI_CORRECTION_NOT_MOVED, // not changed by correction
    FE_GOUCT_CLUMP_CORRECTION_FROM, // this move gets moved elsewhere
    FE_GOUCT_CLUMP_CORRECTION_TO,   // some other move gets moved here
    FE_GOUCT_CLUMP_CORRECTION_NOT_MOVED, // not changed by correction
    FE_MC_OWNER_1, // 0−7 wins/63 sim.
    FE_MC_OWNER_2, // 8−15
    FE_MC_OWNER_3, // 16−23
    FE_MC_OWNER_4, // 24−31
    FE_MC_OWNER_5, // 32−39
    FE_MC_OWNER_6, // 40−47
    FE_MC_OWNER_7, // 48−55
    FE_MC_OWNER_8,  // 56−63
    FE_POS_1, // Position of a point p according to GoBoard::Pos(p)
    FE_POS_2,
    FE_POS_3,
    FE_POS_4,
    FE_POS_5,
    FE_POS_6,
    FE_POS_7,
    FE_POS_8,
    FE_POS_9,
    FE_POS_10,
    FE_GAME_PHASE_1, // Game phase as in Wistuba - 30 moves per phase
    FE_GAME_PHASE_2,
    FE_GAME_PHASE_3,
    FE_GAME_PHASE_4,
    FE_GAME_PHASE_5,
    FE_GAME_PHASE_6,
    FE_GAME_PHASE_7,
    FE_GAME_PHASE_8,
    FE_GAME_PHASE_9,
    FE_GAME_PHASE_10,
    FE_GAME_PHASE_11,
    FE_GAME_PHASE_12,
    FE_NONE,
    _NU_FE_FEATURES

    // other ideas: cfgdistance. tacticaldistance = less for lowlib stones
};

typedef std::bitset<_NU_FE_FEATURES> FeBasicFeatureSet;

std::ostream& operator<<(std::ostream& stream, FeBasicFeature f);

//----------------------------------------------------------------------------

namespace FeFeatures {

const int INVALID_3x3_INDEX = -1;

typedef vector<int>::const_iterator FeIterator;

//---------------------------------

struct FeEvalDetail
{
    FeEvalDetail(int feature, float w, float v);
    int m_feature;
    float m_w;
    float m_v_sum;
};

inline FeEvalDetail::FeEvalDetail(int feature, float w, float v)
    :
    m_feature(feature),
    m_w(w),
    m_v_sum(v)
{ }

std::ostream& operator<<(std::ostream& stream, const FeEvalDetail& f);

//---------------------------------
class FeFeatureWeights
{
public:
    FeFeatureWeights(size_t nuFeatures, size_t k);

    bool IsAllocated() const;

    /** Combine v-values of features i and j */
    float Combine(int i, int j) const;

    size_t m_nuFeatures;

    size_t m_k;

    // length m_nuFeatures
    vector<float> m_w;

    // length k of length m_nuFeatures
    // todo other order should be more cache friendly.
    vector<vector<float> > m_v;
};

std::ostream& operator<<(std::ostream& stream,
                         const FeFeatureWeights& w);
    
//---------------------------------

inline float FeFeatureWeights::Combine(int i, int j) const
{
//    if (   static_cast<size_t>(i) >= m_w.size()
//        || static_cast<size_t>(j) >= m_w.size())
//    {
//        SgDebug() << i << ' ' << j << ' ' << m_w.size() << std::endl;
//    }
    SG_ASSERT(static_cast<size_t>(i) < m_w.size());
    SG_ASSERT(static_cast<size_t>(j) < m_w.size());
    float sum = 0.0;
    for (size_t k = 0; k < m_k; ++k)
    {
        SG_ASSERT(m_v[k].size() == m_w.size());
        sum += m_v[k][i] * m_v[k][j];
    }
    return sum;
}

//---------------------------------
struct FeMoveFeatures
{
    FeMoveFeatures();

    FeBasicFeatureSet m_basicFeatures;
    int m_3x3Index;

};
    
inline FeMoveFeatures::FeMoveFeatures()
    :
    m_basicFeatures(),
    m_3x3Index(INVALID_3x3_INDEX)
{ }

//---------------------------------
/** List of features */
std::vector<int> ActiveFeatures(const FeMoveFeatures& features);

/** Evaluation of given list of features, using weights */
float EvaluateActiveFeatures(const std::vector<int>& active,
                             const FeFeatureWeights& weights);

SgPointArray<float> EvaluateFeatures(const GoBoard& bd,
                             const SgPointArray<FeMoveFeatures>& features,
                             const FeFeatureWeights& weights);

float EvaluateMoveFeatures(const FeMoveFeatures& features,
                           const FeFeatureWeights& weights);

std::vector<FeEvalDetail>
EvaluateMoveFeaturesDetail(const FeMoveFeatures& features,
                           const FeFeatureWeights& weights);

void FindAllFeatures(const GoBoard& bd,
                     SgPointArray<FeMoveFeatures>& features,
                     FeMoveFeatures& passFeatures);

void FindBasicMoveFeatures(const GoBoard& bd, SgPoint move,
                           FeBasicFeatureSet& features);

/** Inefficient, calls full board function, use only for UI */
void FindMoveFeaturesUI(const GoBoard& bd, SgPoint move,
                        FeFeatures::FeMoveFeatures& features);

int Get3x3Feature(const GoBoard& bd, SgPoint p);

void WriteBoardFeatures(std::ostream& stream,
                        const SgPointArray<FeMoveFeatures>& features,
                        const GoBoard& bd);

void WriteEvalDetail(std::ostream& stream,
                     const std::vector<FeEvalDetail>& detail);

void WriteFeatures(std::ostream& stream,
                   SgPoint move,
                   const FeMoveFeatures& features);
    
/** Write features for single move as integer list */
void WriteFeatureSet(std::ostream& stream,
                     SgPoint move,
                     const FeBasicFeatureSet& features);

/** Write features for single move in human-readable strings */
void WriteFeatureSetAsText(std::ostream& stream,
                           SgPoint move,
                           const FeBasicFeatureSet& features);

//----------------------------------------------------------------------------

namespace WistubaFormat {
    
/** Read features in the format produced by Wistuba's tool. */
FeFeatureWeights ReadFeatureWeights(std::istream& stream);
    
/** Write features in the format of Wistuba's gamma learning code
    Each candidate move is described by a list of ID of its features.
    The first number in each line is a 0 for a non-plyed move,
    and 1 for the played move which must be written last.
 
    Example output:
    0 23 456 1 9
    0 45 67 999
    1 55 87 1234
 
    If writeComment is true, then the validator comment #X_Y Z is added.
    Here X is supposed to be a game number, which is not available in this 
    context. Therefore, a 0 is written and later replaced by a standalone
    postprocessing tool.
    Y is the move number.
    Z is the size of the largest matching pattern feature.
    Right now it is set to constant 3.
*/
void WriteBoardFeatures(std::ostream& stream,
                        const SgPointArray<FeMoveFeatures>& features,
                        const FeMoveFeatures& passFeatures,
                        const GoBoard& bd,
                        SgPoint bestMove,
                        bool writeComment);

/** Computes features for the position - preceding - the current one
    and writes them using WriteBoardFeaturesWistuba.
    The computation cannot be for the current positon when used as a GTP
    command since the next move is not available to the engine, but it is
    needed since we need the chosen move. */
    void WriteFeatures(std::ostream& stream,
                       const GoBoard& bd,
                       bool writeComment);
} // namespace WistubaFormat
} // namespace FeBasicFeatures

//----------------------------------------------------------------------------

#endif // FE_BASIC_FEATURES_H

