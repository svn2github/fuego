//----------------------------------------------------------------------------
/** @file FeCommands.cpp  */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FeCommands.h"

#include "FeBasicFeatures.h"
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "SgPointArray.h"
#include "SgWrite.h"

//----------------------------------------------------------------------------
namespace {
} // namespace

FeCommands::FeCommands(const GoBoard& bd//,
                       //GoPlayer*& player,
                       //const GoGame& game
                      )
:   m_bd(bd)//,
//        m_player(player),
//        m_game(game)
{ }

void FeCommands::AddGoGuiAnalyzeCommands(GtpCommand& cmd)
{
    cmd <<
    "sboard/Features/features\n";
}

void FeCommands::CmdFeatures(GtpCommand& cmd)
{
    SgPointArray<FeBasicFeatureSet> features;
    FeBasicFeatureSet passFeatures;
    FeBasicFeatures::FindAllBasicFeatures(m_bd, features, passFeatures);
    cmd << '\n';
    FeBasicFeatures::WriteBoardFeatures(cmd, features, m_bd.Size());
    FeBasicFeatures::WriteFeatureSet(cmd, SG_PASS, passFeatures);
}

void FeCommands::Register(GtpEngine& e)
{
    Register(e, "features", &FeCommands::CmdFeatures);
}

void FeCommands::Register(GtpEngine& engine, const std::string& command,
                             GtpCallback<FeCommands>::Method method)
{
    engine.Register(command, new GtpCallback<FeCommands>(this, method));
}

