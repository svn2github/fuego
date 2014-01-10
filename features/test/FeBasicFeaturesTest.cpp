//----------------------------------------------------------------------------
/** @file FeBasicFeaturesTest.cpp
 Unit tests for FeBasicFeatures. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "FeBasicFeatures.h"

#include "GoBoard.h"
#include "GoSetupUtil.h"
#include "SgDebug.h"
#include "SgWrite.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

FeBasicFeatureSet AtariFeatures() // TODO make it a static variable?
{
    FeBasicFeatureSet features;
    for (FeBasicFeature f = FE_ATARI_LADDER; f <= FE_ATARI_OTHER; ++f)
        features.set(f);
    return features;
}

FeBasicFeatureSet CaptureFeatures() // TODO make it a static variable?
{
    FeBasicFeatureSet features;
    for (FeBasicFeature f = FE_CAPTURE_ADJ_ATARI; f <= FE_CAPTURE_LADDER; ++f)
        features.set(f);
    return features;
}

FeBasicFeatureSet ExtensionFeatures() // TODO make it a static variable?
{
    FeBasicFeatureSet features;
    for (FeBasicFeature f = FE_EXTENSION_NOT_LADDER; f <= FE_EXTENSION_LADDER;
         ++f)
        features.set(f);
    return features;
}

/** Check that f is the only feature in group that is set in features */
inline void TestSingle(FeBasicFeatureSet features,
                       FeBasicFeatureSet group,
                       FeBasicFeature f)
{
    SG_ASSERT(group.test(f));
    BOOST_CHECK(features.test(f));
    BOOST_CHECK_EQUAL((features & group).count(), 1);
}

inline void TestNone(FeBasicFeatureSet features,
                     FeBasicFeatureSet group)
{
    BOOST_CHECK((features & group).none());
}

FeBasicFeatureSet PrevMoveFeatures() // TODO make it a static variable?
{
    FeBasicFeatureSet features;
    for (FeBasicFeature f = FE_DIST_PREV_2; f <= FE_DIST_PREV_17; ++f)
        features.set(f);
    return features;
}

FeBasicFeatureSet PrevOwnMoveFeatures() // TODO make it a static variable?
{
    FeBasicFeatureSet features;
    for (FeBasicFeature f = FE_DIST_PREV_OWN_2;
         f <= FE_DIST_PREV_OWN_17; ++f)
        features.set(f);
    return features;
}

FeBasicFeatureSet PassFeatures() // TODO make it a static variable?
{
    FeBasicFeatureSet features;
    for (FeBasicFeature f = FE_PASS_NEW; f <= FE_PASS_CONSECUTIVE; ++f)
        features.set(f);
    return features;
}

} // namespace

//----------------------------------------------------------------------------

namespace {
    
BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_Line)
{
    GoBoard bd(9);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 1), features);
        BOOST_CHECK(features.test(FE_LINE_1));
        BOOST_CHECK(! features.test(FE_LINE_2));
        BOOST_CHECK(! features.test(FE_LINE_3));
        BOOST_CHECK(! features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 1), features);
        BOOST_CHECK(features.test(FE_LINE_1));
        BOOST_CHECK(! features.test(FE_LINE_2));
        BOOST_CHECK(! features.test(FE_LINE_3));
        BOOST_CHECK(! features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 2), features);
        BOOST_CHECK(! features.test(FE_LINE_1));
        BOOST_CHECK(features.test(FE_LINE_2));
        BOOST_CHECK(! features.test(FE_LINE_3));
        BOOST_CHECK(! features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 5), features);
        BOOST_CHECK(! features.test(FE_LINE_1));
        BOOST_CHECK(features.test(FE_LINE_2));
        BOOST_CHECK(! features.test(FE_LINE_3));
        BOOST_CHECK(! features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 3), features);
        BOOST_CHECK(! features.test(FE_LINE_1));
        BOOST_CHECK(! features.test(FE_LINE_2));
        BOOST_CHECK(features.test(FE_LINE_3));
        BOOST_CHECK(! features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(7, 6), features);
        BOOST_CHECK(! features.test(FE_LINE_1));
        BOOST_CHECK(! features.test(FE_LINE_2));
        BOOST_CHECK(features.test(FE_LINE_3));
        BOOST_CHECK(! features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(6, 4), features);
        BOOST_CHECK(! features.test(FE_LINE_1));
        BOOST_CHECK(! features.test(FE_LINE_2));
        BOOST_CHECK(! features.test(FE_LINE_3));
        BOOST_CHECK(features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(5, 5), features);
        BOOST_CHECK(! features.test(FE_LINE_1));
        BOOST_CHECK(! features.test(FE_LINE_2));
        BOOST_CHECK(! features.test(FE_LINE_3));
        BOOST_CHECK(! features.test(FE_LINE_4));
    }
}

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_DistancePreviousMove)
{
    GoBoard bd(9);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 1), features);
        TestNone(features, PrevMoveFeatures());
    }
    bd.Play(Pt(1,1), SG_BLACK);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 2), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_2);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 3), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_4);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 4), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_6);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 5), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_8);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 6), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_10);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 7), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_12);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 8), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_14);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 9), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_16);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 2), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_3);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 3), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_5);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 4), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_7);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 5), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_9);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 6), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_11);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 7), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_13);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 8), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_15);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 9), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_17);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 3), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_6);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 4), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_8);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 5), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_10);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 6), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_12);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 7), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_14);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 8), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_16);
    }
    for (int i = 3; i <= 9; ++i)
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(i, 9), features);
        TestNone(features, PrevMoveFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(4, 4), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_9);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(4, 5), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_11);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(4, 6), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_13);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(4, 7), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_15);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(4, 8), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_17);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(5, 5), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_12);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(5, 6), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_14);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(5, 7), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_16);
    }
    for (int i = 5; i <= 9; ++i)
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(i, 8), features);
        TestNone(features, PrevMoveFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(6, 6), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_15);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(6, 7), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_17);
    }
    for (int i = 7; i <= 9; ++i)
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(i, 7), features);
        TestNone(features, PrevMoveFeatures());
    }
   
    //-------------------------------------
    {   // no second-last move exists
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 2), features);
        TestNone(features, PrevOwnMoveFeatures());
    }
}

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_DistancePreviousOwnMove)
{
    GoBoard bd(9);
    bd.Play(Pt(1,1), SG_BLACK);
    bd.Play(SG_PASS, SG_WHITE);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 2), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_2);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 3), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_4);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 4), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_6);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 5), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_8);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 6), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_10);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 7), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_12);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 8), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_14);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(1, 9), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_16);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 2), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_3);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 3), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_5);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 4), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_7);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 5), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_9);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 6), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_11);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 7), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_13);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 8), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_15);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 9), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_17);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 3), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_6);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 4), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_8);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 5), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_10);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 6), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_12);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 7), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_14);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 8), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_16);
    }
    for (int i = 3; i <= 9; ++i)
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(i, 9), features);
        TestNone(features, PrevOwnMoveFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(4, 4), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_9);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(4, 5), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_11);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(4, 6), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_13);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(4, 7), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_15);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(4, 8), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_17);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(5, 5), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_12);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(5, 6), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_14);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(5, 7), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_16);
    }
    for (int i = 5; i <= 9; ++i)
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(i, 8), features);
        TestNone(features, PrevOwnMoveFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(6, 6), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_15);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(6, 7), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_17);
    }
    for (int i = 7; i <= 9; ++i)
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(i, 7), features);
        TestNone(features, PrevOwnMoveFeatures());
    }
}

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_Pass)
{
    GoBoard bd(9);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(6, 7), features);
        TestNone(features, PassFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, SG_PASS, features);
        TestSingle(features, PassFeatures(), FE_PASS_NEW);
    }
    bd.Play(Pt(1,1), SG_BLACK);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(6, 7), features);
        TestNone(features, PassFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, SG_PASS, features);
        TestSingle(features, PassFeatures(), FE_PASS_NEW);
    }
    bd.Play(SG_PASS, SG_WHITE);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(6, 7), features);
        TestNone(features, PassFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, SG_PASS, features);
        TestSingle(features, PassFeatures(), FE_PASS_CONSECUTIVE);
    }
}

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_Capture_Adj_Atari)
{
    std::string s("......\n"
                  "......\n"
                  "......\n"
                  "OOOOO.\n"
                  "XXX.X.\n"
                  "XXX.X.");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_WHITE;
    GoBoard bd(boardSize, setup);
    bd.Play(Pt(4, 1), SG_WHITE);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, SG_PASS, features);
        TestNone(features, CaptureFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(6, 1), features);
        TestNone(features, CaptureFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(4, 2), features);
        TestSingle(features, CaptureFeatures(), FE_CAPTURE_ADJ_ATARI);
    }
}

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_Capture_Recapture)
{
    std::string s("......\n"
                  "......\n"
                  "......\n"
                  "......\n"
                  "OOOXX.\n"
                  "XXX.X.");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_WHITE;
    GoBoard bd(boardSize, setup);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(4, 1), features);
        TestSingle(features, CaptureFeatures(), FE_CAPTURE_NOT_LADDER);
    }
    bd.Play(Pt(4, 1), SG_WHITE);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 1), features);
        TestSingle(features, CaptureFeatures(), FE_CAPTURE_RECAPTURE);
    }
}

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_Capture_Recapture_2)
{
    std::string s("......\n"
                  "......\n"
                  "......\n"
                  "......\n"
                  "OOOXXX\n"
                  "XXX.OX");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_WHITE;
    GoBoard bd(boardSize, setup);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(4, 1), features);
        TestSingle(features, CaptureFeatures(), FE_CAPTURE_NOT_LADDER);
    }
    bd.Play(Pt(4, 1), SG_WHITE);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 1), features);
        TestSingle(features, CaptureFeatures(), FE_CAPTURE_RECAPTURE);
    }
}

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_Capture_Ladder)
{
    std::string s("......\n"
                  "......\n"
                  "......\n"
                  "O.....\n"
                  "OXO...\n"
                  ".O....");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_WHITE;
    GoBoard bd(boardSize, setup);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 3), features);
        TestSingle(features, CaptureFeatures(), FE_CAPTURE_LADDER);
    }
    bd.Play(Pt(5, 5), SG_BLACK);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 3), features);
        TestSingle(features, CaptureFeatures(), FE_CAPTURE_NOT_LADDER);
    }
}

/*
BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_Capture_PreventConnection)
{ // TODO feature not completely implemented yet.
}

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_Capture_Multiple)
{ // TODO feature not implemented yet.
}
*/

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_Extension)
{
    std::string s("......\n"
                  "......\n"
                  "......\n"
                  "O.....\n"
                  "OX....\n"
                  ".O....");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_WHITE;
    GoBoard bd(boardSize, setup);
    bd.Play(Pt(3, 2), SG_WHITE);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 3), features);
        TestSingle(features, ExtensionFeatures(), FE_EXTENSION_LADDER);
    }
    bd.Undo();
    bd.Play(SG_PASS, SG_WHITE);
    bd.Play(Pt(5, 5), SG_BLACK); // break the ladder
    bd.Play(Pt(3, 2), SG_WHITE);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 3), features);
        TestSingle(features, ExtensionFeatures(), FE_EXTENSION_NOT_LADDER);
    }
}

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_SelfAtari)
{
    std::string s("...O..\n"
                  "..XXOO\n"
                  ".O.XO.\n"
                  "O.OXOO\n"
                  "OX.XX.\n"
                  ".O....");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    GoBoard bd(boardSize, setup);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 3), features);
        BOOST_CHECK(features.test(FE_SELFATARI));
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(5, 6), features);
        BOOST_CHECK(features.test(FE_SELFATARI));
        // TODO this is a good selfatari FE_SELFATARI_NAKADE,
        // FE_SELFATARI_THROWIN
    }
}

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_SelfAtari_2)
{
    std::string s("...O.X\n"
                  "...OXX\n"
                  "...OO.\n"
                  "....OO\n"
                  "......\n"
                  "......");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    GoBoard bd(boardSize, setup);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(6, 4), features);
        BOOST_CHECK(features.test(FE_SELFATARI));
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(5, 6), features);
        BOOST_CHECK(features.test(FE_SELFATARI));
    }
}

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_Atari_Ladder)
{
    std::string s("......\n"
                  "......\n"
                  "......\n"
                  "O.....\n"
                  "OX....\n"
                  ".O....");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_WHITE;
    GoBoard bd(boardSize, setup);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 2), features);
        TestSingle(features, AtariFeatures(), FE_ATARI_LADDER);
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 3), features);
        TestSingle(features, AtariFeatures(), FE_ATARI_OTHER);
    }
}

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_Atari_Ko)
{
    std::string s("...O.O\n"
                  "....OX\n"
                  "......\n"
                  "O.....\n"
                  "OX....\n"
                  ".O....");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    GoBoard bd(boardSize, setup);
    bd.Play(Pt(5, 6), SG_BLACK);
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(3, 2), features);
        BOOST_CHECK(features.test(FE_ATARI_KO));
    }
    {
        FeBasicFeatureSet features;
        FeBasicFeatures::FindFeatures(bd, Pt(2, 3), features);
        BOOST_CHECK(features.test(FE_ATARI_KO));
    }
}

} // namespace

//----------------------------------------------------------------------------
/*
FE_MC_OWNER_1, // 0−7 wins/63 sim.
FE_MC_OWNER_2, // 8−15
FE_MC_OWNER_3, // 16−23
FE_MC_OWNER_4, // 24−31
FE_MC_OWNER_5, // 32−39
FE_MC_OWNER_6, // 40−47
FE_MC_OWNER_7, // 48−55
FE_MC_OWNER_8,  // 56−63
*/