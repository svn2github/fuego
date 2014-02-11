//----------------------------------------------------------------------------
/** @file FeCommands.cpp  */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FeCommands.h"

#include <fstream>
#include "FeBasicFeatures.h"
#include "FePattern.h"
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoGame.h"
#include "GoPlayer.h"
#include "GoGtpCommandUtil.h"
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
        m_weights(0, 0)
{
    SG_UNUSED(m_player);
    SG_UNUSED(m_game);
}

void FeCommands::AddGoGuiAnalyzeCommands(GtpCommand& cmd)
{
    cmd <<
    "none/Features/features\n"
    "none/Features Define Pattern/features_define_pattern\n"
    "cboard/Features Evaluate Board/features_evaluate_board\n"
    "none/Features Move/features_move %p\n"
    "none/Features Read Weights/features_read_weights %r\n"
    "none/Features Wistuba/features_wistuba\n"
    "none/Features Wistuba - File/features_wistuba_file\n"
    "none/Features+Comments Wistuba - File/features_comments_wistuba_file\n"
    ;
}

void FeCommands::CheckWeights(std::string message) const
{
    if (m_weights.m_nuFeatures == 0)
        throw SgException(message + ": need to call"
                          " features_read_weights first ");

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

void FeCommands::CmdFeaturesEvaluateBoard(GtpCommand& cmd)
{
    using namespace FeFeatures;
    CheckWeights("features_evaluate_board");

    SgPointArray<FeMoveFeatures> features;
    FeMoveFeatures passFeatures;
    FindAllFeatures(m_bd, features, passFeatures);
    SgPointArray<float> eval = EvaluateFeatures(m_bd, features, m_weights);
    float passEval = EvaluateMoveFeatures(passFeatures, m_weights);
    cmd << '\n';
    //cmd << SgWritePointArrayFloat<float>(eval, m_bd.Size(), true, 2);
    GoGtpCommandUtil::RespondColorGradientData(cmd, eval,
                                  eval.MinValue(),
                                  eval.MaxValue(),
                                  m_bd);

    SgDebug() << "Eval min = " << eval.MinValue()
              << ", max = " << eval.MaxValue() << '\n';
    
    cmd << "Pass: " << passEval << '\n';
}

void FeCommands::CmdFeaturesMove(GtpCommand& cmd)
{
    using namespace FeFeatures;
    CheckWeights("features_move");

    SgPoint move = GoGtpCommandUtil::MoveArg(cmd, 0, m_bd);
    FeMoveFeatures f;
    FindMoveFeaturesUI(m_bd, move, f);
    std::vector<FeEvalDetail> detail =
    EvaluateMoveFeaturesDetail(f, m_weights);
    WriteEvalDetail(SgDebug(), detail);
    SgDebug() << std::endl;
}

void FeCommands::CmdFeaturesReadWeights(GtpCommand& cmd)
{
    const std::string fileName = cmd.Arg();
    try
    {
        std::ifstream in(fileName.c_str());
        if (! in)
        throw SgException("Cannot find file " + fileName);
        m_weights =
            FeFeatures::WistubaFormat::ReadFeatureWeights(in);
    }
    catch (const SgException& e)
    {
        throw GtpFailure() << "loading features file failed: " << e.what();
    }
    SgDebug() << "Read weights for " << m_weights.m_nuFeatures
              << " features with k = " << m_weights.m_k << '\n';
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

/** Write features and comments for validation in Wistuba's format */
void FeCommands::CmdFeaturesDefinePattern(GtpCommand& cmd)
{
    SG_UNUSED(cmd);

    for (PaAxSet s = 0; s < PA_NU_AX_SETS; ++s)
    {
        PaAx ax(s);
        SgPoint corner = AXBoardToBoard(9, 9, ax, m_bd.Size());
        int x = SgPointUtil::Col(corner);
        int y = SgPointUtil::Row(corner);
        PaSpot spot(x, y, ax, false);
        int width = 5;
        int height = 7;
        FeRectPattern* p = DefineRectPattern(m_bd, spot, width, height);
        SgDebug() << *p;
        delete p;
    }
}

void FeCommands::Register(GtpEngine& e)
{
    Register(e, "features", &FeCommands::CmdFeatures);
    Register(e, "features_define_pattern",
             &FeCommands::CmdFeaturesDefinePattern);
    Register(e, "features_evaluate_board",
             &FeCommands::CmdFeaturesEvaluateBoard);
    Register(e, "features_move", &FeCommands::CmdFeaturesMove);
    Register(e, "features_read_weights", &FeCommands::CmdFeaturesReadWeights);
    Register(e, "features_wistuba", &FeCommands::CmdFeaturesWistuba);
    Register(e, "features_wistuba_file",
             &FeCommands::CmdFeaturesWistubaToFile);
    Register(e, "features_comments_wistuba_file",
             &FeCommands::CmdFeaturesCommentsWistubaToFile);
}

void FeCommands::Register(GtpEngine& engine,
                          const std::string& command,
                          GtpCallback<FeCommands>::Method method)
{
    engine.Register(command, new GtpCallback<FeCommands>(this, method));
}

