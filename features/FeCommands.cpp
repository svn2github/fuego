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
namespace {
    


} // namespace

FeCommands::FeCommands(const GoBoard& bd,
                       GoPlayer*& player,
                       const GoGame& game)
    :   m_bd(bd),
        m_player(player),
        m_game(game)
{
    SG_UNUSED(m_player);
}

void FeCommands::AddGoGuiAnalyzeCommands(GtpCommand& cmd)
{
    cmd <<
    "none/Features/features\n"
    "none/Features Wistuba Format/features_wistuba\n"
    "none/Features Wistuba Format To File/features_wistuba_file\n"
    ;
}

void FeCommands::CmdFeatures(GtpCommand& cmd)
{
    SgPointArray<FeBasicFeatureSet> features;
    FeBasicFeatureSet passFeatures;
    FeBasicFeatures::FindAllBasicFeatures(m_bd, features, passFeatures);
    cmd << '\n';
    FeBasicFeatures::WriteBoardFeatures(cmd, features, m_bd);
    FeBasicFeatures::WriteFeatureSet(cmd, SG_PASS, passFeatures);
}

void FeCommands::CmdFeaturesWistuba(GtpCommand& cmd)
{
    cmd << '\n';
    FeBasicFeatures::WriteFeaturesWistuba(cmd, m_bd);

    SgDebug()
        << "CurrentMoveNumber = " << m_game.CurrentMoveNumber()
        << '\n';
}

void FeCommands::CmdFeaturesWistubaToFile(GtpCommand& cmd)
{
    cmd << '\n';
    std::ofstream stream("features.txt", std::ios::app);
    FeBasicFeatures::WriteFeaturesWistuba(stream, m_bd);
    //stream << std::flush;

    SgDebug() << m_bd
    << "m_game.CurrentMoveNumber() " << m_game.CurrentMoveNumber()
    << '\n';
}

void FeCommands::Register(GtpEngine& e)
{
    Register(e, "features", &FeCommands::CmdFeatures);
    Register(e, "features_wistuba", &FeCommands::CmdFeaturesWistuba);
    Register(e, "features_wistuba_file",
             &FeCommands::CmdFeaturesWistubaToFile);
}

void FeCommands::Register(GtpEngine& engine, const std::string& command,
                             GtpCallback<FeCommands>::Method method)
{
    engine.Register(command, new GtpCallback<FeCommands>(this, method));
}

