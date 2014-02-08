//----------------------------------------------------------------------------
/** @file FeCommands.cpp  */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FeCommands.h"

#include <fstream>
#include "FeBasicFeatures.h"
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoGame.h"
#include "GoPlayer.h"
#include "SgPointArray.h"
#include "SgNode.h"
#include "SgWrite.h"

//----------------------------------------------------------------------------

FeCommands::FeCommands(const GoBoard& bd,
                       GoPlayer*& player,
                       const GoGame& game)
    :   m_bd(bd),
        m_player(player),
        m_game(game),
        m_features(0, 0)
{
    SG_UNUSED(m_player);
    SG_UNUSED(m_game);
}

void FeCommands::AddGoGuiAnalyzeCommands(GtpCommand& cmd)
{
    cmd <<
    "none/Features/features\n"
    "none/Features Read Weights/features_read_weights\n"
    "none/Features Wistuba/features_wistuba\n"
    "none/Features Wistuba - File/features_wistuba_file\n"
    "none/Features+Comments Wistuba - File/features_comments_wistuba_file\n"
    ;
}

void FeCommands::CmdFeatures(GtpCommand& cmd)
{
    using FeFeatures::FeMoveFeatures;
    SgPointArray<FeMoveFeatures> features;
    FeMoveFeatures passFeatures;
    FeFeatures::FindAllFeatures(m_bd, features, passFeatures);
    cmd << '\n';
    FeFeatures::WriteBoardFeatures(cmd, features, m_bd);
    FeFeatures::WriteFeatures(cmd, SG_PASS, passFeatures);
}

void FeCommands::CmdFeaturesReadWeights(GtpCommand& cmd)
{
    const std::string fileName = cmd.Arg();
    try
    {
        std::ifstream in(fileName.c_str());
        if (! in)
        throw SgException("Cannot find file " + fileName);
        m_features =
            FeFeatures::WistubaFormat::ReadFeatureWeights(in);
    }
    catch (const SgException& e)
    {
        throw GtpFailure() << "loading features file failed: " << e.what();
    }
    SgDebug() << "Read weights for " << m_features.m_nuFeatures
              << "features with k = " << m_features.m_k << '\n';
}

void FeCommands::CmdFeaturesWistuba(GtpCommand& cmd)
{
    cmd << '\n';
    FeFeatures::WistubaFormat::WriteFeatures(cmd, m_bd, true);
}

/** Write features and possibly comments for validation in Wistuba's format */
void FeCommands::FeaturesWistubaToFile(GtpCommand& cmd, bool writeComments)
{
    cmd << '\n';
    std::ofstream stream("features.txt", std::ios::app);
    FeFeatures::WistubaFormat::WriteFeatures(stream, m_bd, writeComments);
}

/** Write features only in Wistuba's format */
void FeCommands::CmdFeaturesWistubaToFile(GtpCommand& cmd)
{
    FeaturesWistubaToFile(cmd, false);
}

/** Write features and comments for validation in Wistuba's format */
void FeCommands::CmdFeaturesCommentsWistubaToFile(GtpCommand& cmd)
{
    FeaturesWistubaToFile(cmd, true);
}

void FeCommands::Register(GtpEngine& e)
{
    Register(e, "features", &FeCommands::CmdFeatures);
    Register(e, "features_read_weights", &FeCommands::CmdFeaturesReadWeights);
    Register(e, "features_wistuba", &FeCommands::CmdFeaturesWistuba);
    Register(e, "features_wistuba_file",
             &FeCommands::CmdFeaturesWistubaToFile);
    Register(e, "features_comments_wistuba_file",
             &FeCommands::CmdFeaturesCommentsWistubaToFile);
}

void FeCommands::Register(GtpEngine& engine, const std::string& command,
                             GtpCallback<FeCommands>::Method method)
{
    engine.Register(command, new GtpCallback<FeCommands>(this, method));
}

