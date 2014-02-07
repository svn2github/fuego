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

/** Add contiguous range to set of features. Includes all [first..last] */
void AddRange(FeBasicFeatureSet& features, FeBasicFeature first,
              FeBasicFeature last)
{
    for (int f = static_cast<int>(first);
             f <= static_cast<int>(last); ++f)
        features.set(f);
}

// TODO make sets into static variables?
FeBasicFeatureSet AtariFeatures()
{
    FeBasicFeatureSet features;
    AddRange(features, FE_ATARI_LADDER, FE_ATARI_OTHER);
    return features;
}

FeBasicFeatureSet CaptureFeatures()
{
    FeBasicFeatureSet features;
    AddRange(features, FE_CAPTURE_ADJ_ATARI, FE_CAPTURE_LADDER);
    return features;
}

FeBasicFeatureSet ExtensionFeatures()
{
    FeBasicFeatureSet features;
    AddRange(features, FE_EXTENSION_NOT_LADDER, FE_EXTENSION_LADDER);
    return features;
}

FeBasicFeatureSet PassFeatures() // TODO make it a static variable?
{
    FeBasicFeatureSet features;
    AddRange(features, FE_PASS_NEW, FE_PASS_CONSECUTIVE);
    return features;
}

FeBasicFeatureSet PrevMoveFeatures() // TODO make it a static variable?
{
    FeBasicFeatureSet features;
    AddRange(features, FE_DIST_PREV_2, FE_DIST_PREV_17);
    return features;
}

FeBasicFeatureSet PrevOwnMoveFeatures() // TODO make it a static variable?
{
    FeBasicFeatureSet features;
    AddRange(features, FE_DIST_PREV_OWN_2, FE_DIST_PREV_OWN_17);
    return features;
}

inline void TestNone(FeBasicFeatureSet features,
                     FeBasicFeatureSet group)
{
    BOOST_CHECK((features & group).none());
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

} // namespace

//----------------------------------------------------------------------------

namespace {
    
BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_Line)
{
    GoBoard bd(9);
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 1), features);
        BOOST_CHECK(features.test(FE_LINE_1));
        BOOST_CHECK(! features.test(FE_LINE_2));
        BOOST_CHECK(! features.test(FE_LINE_3));
        BOOST_CHECK(! features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 1), features);
        BOOST_CHECK(features.test(FE_LINE_1));
        BOOST_CHECK(! features.test(FE_LINE_2));
        BOOST_CHECK(! features.test(FE_LINE_3));
        BOOST_CHECK(! features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 2), features);
        BOOST_CHECK(! features.test(FE_LINE_1));
        BOOST_CHECK(features.test(FE_LINE_2));
        BOOST_CHECK(! features.test(FE_LINE_3));
        BOOST_CHECK(! features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 5), features);
        BOOST_CHECK(! features.test(FE_LINE_1));
        BOOST_CHECK(features.test(FE_LINE_2));
        BOOST_CHECK(! features.test(FE_LINE_3));
        BOOST_CHECK(! features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 3), features);
        BOOST_CHECK(! features.test(FE_LINE_1));
        BOOST_CHECK(! features.test(FE_LINE_2));
        BOOST_CHECK(features.test(FE_LINE_3));
        BOOST_CHECK(! features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(7, 6), features);
        BOOST_CHECK(! features.test(FE_LINE_1));
        BOOST_CHECK(! features.test(FE_LINE_2));
        BOOST_CHECK(features.test(FE_LINE_3));
        BOOST_CHECK(! features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(6, 4), features);
        BOOST_CHECK(! features.test(FE_LINE_1));
        BOOST_CHECK(! features.test(FE_LINE_2));
        BOOST_CHECK(! features.test(FE_LINE_3));
        BOOST_CHECK(features.test(FE_LINE_4));
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(5, 5), features);
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
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 1), features);
        TestNone(features, PrevMoveFeatures());
    }
    bd.Play(Pt(1,1), SG_BLACK);
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 2), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_2);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 3), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_4);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 4), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_6);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 5), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_8);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 6), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_10);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 7), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_12);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 8), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_14);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 9), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_16);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 2), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_3);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 3), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_5);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 4), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_7);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 5), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_9);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 6), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_11);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 7), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_13);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 8), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_15);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 9), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_17);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 3), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_6);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 4), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_8);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 5), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_10);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 6), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_12);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 7), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_14);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 8), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_16);
    }
    for (int i = 3; i <= 9; ++i)
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(i, 9), features);
        TestNone(features, PrevMoveFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(4, 4), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_9);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(4, 5), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_11);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(4, 6), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_13);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(4, 7), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_15);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(4, 8), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_17);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(5, 5), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_12);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(5, 6), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_14);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(5, 7), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_16);
    }
    for (int i = 5; i <= 9; ++i)
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(i, 8), features);
        TestNone(features, PrevMoveFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(6, 6), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_15);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(6, 7), features);
        TestSingle(features, PrevMoveFeatures(), FE_DIST_PREV_17);
    }
    for (int i = 7; i <= 9; ++i)
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(i, 7), features);
        TestNone(features, PrevMoveFeatures());
    }
   
    //-------------------------------------
    {   // no second-last move exists
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 2), features);
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
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 2), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_2);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 3), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_4);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 4), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_6);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 5), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_8);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 6), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_10);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 7), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_12);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 8), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_14);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(1, 9), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_16);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 2), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_3);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 3), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_5);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 4), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_7);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 5), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_9);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 6), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_11);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 7), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_13);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 8), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_15);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 9), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_17);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 3), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_6);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 4), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_8);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 5), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_10);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 6), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_12);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 7), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_14);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 8), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_16);
    }
    for (int i = 3; i <= 9; ++i)
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(i, 9), features);
        TestNone(features, PrevOwnMoveFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(4, 4), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_9);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(4, 5), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_11);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(4, 6), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_13);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(4, 7), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_15);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(4, 8), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_17);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(5, 5), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_12);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(5, 6), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_14);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(5, 7), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_16);
    }
    for (int i = 5; i <= 9; ++i)
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(i, 8), features);
        TestNone(features, PrevOwnMoveFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(6, 6), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_15);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(6, 7), features);
        TestSingle(features, PrevOwnMoveFeatures(), FE_DIST_PREV_OWN_17);
    }
    for (int i = 7; i <= 9; ++i)
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(i, 7), features);
        TestNone(features, PrevOwnMoveFeatures());
    }
}

BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_Pass)
{
    GoBoard bd(9);
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(6, 7), features);
        TestNone(features, PassFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, SG_PASS, features);
        TestSingle(features, PassFeatures(), FE_PASS_NEW);
    }
    bd.Play(Pt(1,1), SG_BLACK);
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(6, 7), features);
        TestNone(features, PassFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, SG_PASS, features);
        TestSingle(features, PassFeatures(), FE_PASS_NEW);
    }
    bd.Play(SG_PASS, SG_WHITE);
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(6, 7), features);
        TestNone(features, PassFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, SG_PASS, features);
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
        FeFeatures::FindBasicMoveFeatures(bd, SG_PASS, features);
        TestNone(features, CaptureFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(6, 1), features);
        TestNone(features, CaptureFeatures());
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(4, 2), features);
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
        FeFeatures::FindBasicMoveFeatures(bd, Pt(4, 1), features);
        TestSingle(features, CaptureFeatures(), FE_CAPTURE_NOT_LADDER);
    }
    bd.Play(Pt(4, 1), SG_WHITE);
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 1), features);
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
        FeFeatures::FindBasicMoveFeatures(bd, Pt(4, 1), features);
        TestSingle(features, CaptureFeatures(), FE_CAPTURE_NOT_LADDER);
    }
    bd.Play(Pt(4, 1), SG_WHITE);
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 1), features);
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
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 3), features);
        TestSingle(features, CaptureFeatures(), FE_CAPTURE_LADDER);
    }
    bd.Play(Pt(5, 5), SG_BLACK);
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 3), features);
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
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 3), features);
        TestSingle(features, ExtensionFeatures(), FE_EXTENSION_LADDER);
    }
    bd.Undo();
    bd.Play(SG_PASS, SG_WHITE);
    bd.Play(Pt(5, 5), SG_BLACK); // break the ladder
    bd.Play(Pt(3, 2), SG_WHITE);
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 3), features);
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
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 3), features);
        BOOST_CHECK(features.test(FE_SELFATARI));
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(5, 6), features);
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
        FeFeatures::FindBasicMoveFeatures(bd, Pt(6, 4), features);
        BOOST_CHECK(features.test(FE_SELFATARI));
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(5, 6), features);
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
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 2), features);
        TestSingle(features, AtariFeatures(), FE_ATARI_LADDER);
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 3), features);
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
        FeFeatures::FindBasicMoveFeatures(bd, Pt(3, 2), features);
        BOOST_CHECK(features.test(FE_ATARI_KO));
    }
    {
        FeBasicFeatureSet features;
        FeFeatures::FindBasicMoveFeatures(bd, Pt(2, 3), features);
        BOOST_CHECK(features.test(FE_ATARI_KO));
    }
}

    BOOST_AUTO_TEST_CASE(FeBasicFeaturesTest_3x3_Empty_Board)
    {
        using FeFeatures::Get3x3Feature;
        GoBoard bd(5);
        const int f1 = Get3x3Feature(bd, Pt(3, 3));
        const int f2 = Get3x3Feature(bd, Pt(2, 3));
        const int f3 = Get3x3Feature(bd, Pt(4, 2));
        const int f4 = Get3x3Feature(bd, Pt(4, 4));
        BOOST_CHECK_EQUAL(f1, f2);
        BOOST_CHECK_EQUAL(f1, f3);
        BOOST_CHECK_EQUAL(f1, f4);
        const int f5 = Get3x3Feature(bd, Pt(1, 2));
        BOOST_CHECK(f1 != f5);
        const int f6 = Get3x3Feature(bd, Pt(1, 3));
        const int f7 = Get3x3Feature(bd, Pt(1, 4));
        const int f8 = Get3x3Feature(bd, Pt(2, 1));
        const int f9 = Get3x3Feature(bd, Pt(4, 1));
        const int f10 = Get3x3Feature(bd, Pt(5, 2));
        const int f11 = Get3x3Feature(bd, Pt(5, 3));
        const int f12 = Get3x3Feature(bd, Pt(5, 4));
        const int f13 = Get3x3Feature(bd, Pt(2, 5));
        const int f14 = Get3x3Feature(bd, Pt(3, 5));
        const int f15 = Get3x3Feature(bd, Pt(4, 5));
        BOOST_CHECK_EQUAL(f5, f6);
        BOOST_CHECK_EQUAL(f5, f7);
        BOOST_CHECK_EQUAL(f5, f8);
        BOOST_CHECK_EQUAL(f5, f9);
        BOOST_CHECK_EQUAL(f5, f10);
        BOOST_CHECK_EQUAL(f5, f11);
        BOOST_CHECK_EQUAL(f5, f12);
        BOOST_CHECK_EQUAL(f5, f13);
        BOOST_CHECK_EQUAL(f5, f14);
        BOOST_CHECK_EQUAL(f5, f15);
        const int f16 = Get3x3Feature(bd, Pt(1, 1));
        BOOST_CHECK(f1 != f16);
        BOOST_CHECK(f5 != f16);
        const int f17 = Get3x3Feature(bd, Pt(1, 5));
        const int f18 = Get3x3Feature(bd, Pt(5, 1));
        const int f19 = Get3x3Feature(bd, Pt(5, 5));
        BOOST_CHECK_EQUAL(f17, f16);
        BOOST_CHECK_EQUAL(f18, f16);
        BOOST_CHECK_EQUAL(f19, f16);
    }
    

} // namespace

//----------------------------------------------------------------------------
