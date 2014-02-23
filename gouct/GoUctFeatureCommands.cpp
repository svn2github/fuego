//----------------------------------------------------------------------------
/** @file GoUctFeatureCommands.cpp  */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctFeatureCommands.h"

#include <fstream>
#include "FeBasicFeatures.h"
#include "FeFeatureWeights.h"
#include "FePattern.h"
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoGame.h"
#include "GoPlayer.h"
#include "GoGtpCommandUtil.h"
#include "GoUctFeatures.h"
#include "GoUctPlayoutPolicy.h"
#include "SgPointArray.h"
#include "SgNode.h"
#include "SgWrite.h"

//----------------------------------------------------------------------------

GoUctFeatureCommands::GoUctFeatureCommands(const GoBoard& bd)
    :   m_bd(bd),
        m_weights(FeFeatureWeights::ReadDefaultWeights()),
        m_policy(bd, GoUctPlayoutPolicyParam())
{ }

void GoUctFeatureCommands::AddGoGuiAnalyzeCommands(GtpCommand& cmd)
{
    cmd <<
    "none/Features/features\n"
    "none/Features Define Pattern/features_define_pattern\n"
    "cboard/Features Evaluate Board/features_evaluate_board\n"
    "none/Features Move/features_move %p\n"
    "none/Features Read Weights/features_read_weights %r\n"
    "hstring/Features Write Weights/features_write_weights\n"
    "none/Features Wistuba/features_wistuba\n"
    "none/Features Wistuba - File/features_wistuba_file\n"
    "none/Features+Comments Wistuba - File/features_comments_wistuba_file\n"
    ;
}

void GoUctFeatureCommands::CheckWeights(std::string message) const
{
    if (m_weights.m_nuFeatures == 0)
        throw SgException(message + ": need to call"
                          " features_read_weights first ");

}

void GoUctFeatureCommands::CmdFeatures(GtpCommand& cmd)
{
    FeFullBoardFeatures features(m_bd);
    GoUctFeatures::FindAllFeatures(m_bd, m_policy, features);
    cmd << '\n';
    features.WriteBoardFeatures(cmd);
}

void GoUctFeatureCommands::CmdFeaturesEvaluateBoard(GtpCommand& cmd)
{
    using namespace FeFeatures;
    CheckWeights("features_evaluate_board");

    FeFullBoardFeatures features(m_bd);
    GoUctFeatures::FindAllFeatures(m_bd, m_policy, features);
    GoEvalArray<float> eval = features.EvaluateFeatures(m_weights);
    cmd << '\n';
    //cmd << SgWritePointArrayFloat<float>(eval, m_bd.Size(), true, 2);
    GoGtpCommandUtil::RespondColorGradientData(cmd, eval,
                                  eval.MinValue(),
                                  eval.MaxValue(),
                                  m_bd);

    SgDebug() << "Eval min = " << eval.MinValue()
              << ", max = " << eval.MaxValue() << '\n';
    
    cmd << "Pass: " << eval[SG_PASS] << '\n';
}

void GoUctFeatureCommands::CmdFeaturesMove(GtpCommand& cmd)
{
    using namespace FeFeatures;
    CheckWeights("features_move");

    SgPoint move = GoGtpCommandUtil::MoveArg(cmd, 0, m_bd);
    FeMoveFeatures f;
    GoUctFeatures::FindMoveFeaturesUI(m_bd, m_policy, move, f);
    std::vector<FeEvalDetail> detail =
    EvaluateMoveFeaturesDetail(f, m_weights);
    WriteEvalDetail(SgDebug(), detail);
    SgDebug() << std::endl;
}

void GoUctFeatureCommands::CmdFeaturesReadWeights(GtpCommand& cmd)
{
    const std::string fileName = cmd.Arg();
    try
    {
        std::ifstream in(fileName.c_str());
        if (! in)
        throw SgException("Cannot find file " + fileName);
        m_weights = FeFeatureWeights::Read(in);
    }
    catch (const SgException& e)
    {
        throw GtpFailure() << "loading features file failed: " << e.what();
    }
    SgDebug() << "Read weights for " << m_weights.m_nuFeatures
              << " features with k = " << m_weights.m_k << '\n';
}

void GoUctFeatureCommands::CmdFeaturesWistuba(GtpCommand& cmd)
{
    cmd << '\n';
    GoUctFeatures::WriteFeatures(cmd, m_policy, m_bd, true);
}

/** Write features and possibly comments for validation in Wistuba's format */
void GoUctFeatureCommands::FeaturesWistubaToFile(GtpCommand& cmd,
                                                 bool writeComments)
{
    cmd << '\n';
    std::ofstream stream("features.txt", std::ios::app);
    GoUctFeatures::WriteFeatures(stream, m_policy, m_bd, writeComments);
}

/** Write features only in Wistuba's format */
void GoUctFeatureCommands::CmdFeaturesWistubaToFile(GtpCommand& cmd)
{
    FeaturesWistubaToFile(cmd, false);
}

/** Write features and comments for validation in Wistuba's format */
void GoUctFeatureCommands::CmdFeaturesCommentsWistubaToFile(GtpCommand& cmd)
{
    FeaturesWistubaToFile(cmd, true);
}

/** Write features and comments for validation in Wistuba's format */
void GoUctFeatureCommands::CmdFeaturesDefinePattern(GtpCommand& cmd)
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

void GoUctFeatureCommands::CmdFeaturesWriteWeights(GtpCommand& cmd)
{
    cmd << m_weights;
}

void GoUctFeatureCommands::Register(GtpEngine& e)
{
    Register(e, "features", &GoUctFeatureCommands::CmdFeatures);
    Register(e, "features_define_pattern",
             &GoUctFeatureCommands::CmdFeaturesDefinePattern);
    Register(e, "features_evaluate_board",
             &GoUctFeatureCommands::CmdFeaturesEvaluateBoard);
    Register(e, "features_move", &GoUctFeatureCommands::CmdFeaturesMove);
    Register(e, "features_read_weights",
             &GoUctFeatureCommands::CmdFeaturesReadWeights);
    Register(e, "features_wistuba",
             &GoUctFeatureCommands::CmdFeaturesWistuba);
    Register(e, "features_wistuba_file",
             &GoUctFeatureCommands::CmdFeaturesWistubaToFile);
    Register(e, "features_comments_wistuba_file",
             &GoUctFeatureCommands::CmdFeaturesCommentsWistubaToFile);
    Register(e, "features_write_weights",
             &GoUctFeatureCommands::CmdFeaturesWriteWeights);
}

void GoUctFeatureCommands::Register(GtpEngine& engine,
                          const std::string& command,
                          GtpCallback<GoUctFeatureCommands>::Method method)
{
    engine.Register(command,
                    new GtpCallback<GoUctFeatureCommands>(this, method));
}

