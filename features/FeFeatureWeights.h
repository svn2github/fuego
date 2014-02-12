//----------------------------------------------------------------------------
/** @file FeFeatureWeights.h
     Weights of features as described in:
 Move Prediction in Go – Modelling Feature Interactions Using Latent Factors
 Martin Wistuba and Lars Schmidt-Thieme
 KI 2013
*/
//----------------------------------------------------------------------------

#ifndef FE_FEATURE_WEIGHTS_H
#define FE_FEATURE_WEIGHTS_H

#include <iosfwd>
#include <vector>

//----------------------------------------------------------------------------

class FeFeatureWeights
{
public:
    static const size_t MAX_FEATURE_INDEX;

    FeFeatureWeights(size_t nuFeatures, size_t k);

    bool IsAllocated() const;

    /** Combine v-values of features i and j */
    float Combine(int i, int j) const;

    /** Read features in the format produced by Wistuba's tool. */
    static FeFeatureWeights Read(std::istream& stream);

    size_t m_nuFeatures;

    size_t m_k;

    // length m_nuFeatures
    std::vector<float> m_w;

    // length k of length m_nuFeatures
    // todo other order should be more cache friendly.
    std::vector<std::vector<float> > m_v;
};

//----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream,
                         const FeFeatureWeights& w);

//----------------------------------------------------------------------------

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

//----------------------------------------------------------------------------

#endif // FE_FEATURE_WEIGHTS_H
