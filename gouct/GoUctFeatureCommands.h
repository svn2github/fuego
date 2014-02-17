//----------------------------------------------------------------------------
/** @file GoUctFeatureCommands.h
    GTP commands for features. */
//----------------------------------------------------------------------------

#ifndef FE_COMMANDS_H
#define FE_COMMANDS_H

#include <iostream>
#include <string>
#include "GtpEngine.h"

#include "FeBasicFeatures.h"
#include "FeFeatureWeights.h"
#include "GoUctPlayoutPolicy.h"

class GoBoard;

/** GTP commands for features. */
class GoUctFeatureCommands
{
public:
    /** Constructor.
     @param bd The game board.
     @param player Reference to pointer to current player. Can be null. */
    GoUctFeatureCommands(const GoBoard& bd);

    void AddGoGuiAnalyzeCommands(GtpCommand& cmd);

    /** @name Command Callbacks */
    // @{
    // The callback functions are documented in the cpp file
    void CmdFeatures(GtpCommand& cmd);

    void CmdFeaturesDefinePattern(GtpCommand& cmd);
    
    void CmdFeaturesEvaluateBoard(GtpCommand& cmd);

    void CmdFeaturesMove(GtpCommand& cmd);

    /** Read weights from a file */
    void CmdFeaturesReadWeights(GtpCommand& cmd);
    
    void CmdFeaturesWistuba(GtpCommand& cmd);

    void CmdFeaturesWistubaToFile(GtpCommand& cmd);

    void CmdFeaturesCommentsWistubaToFile(GtpCommand& cmd);

    void Register(GtpEngine& engine);

private:
    void CheckWeights(std::string message) const;

    void FeaturesWistubaToFile(GtpCommand& cmd, bool writeComments);
    
    void Register(GtpEngine& engine,
                  const std::string& command,
                  GtpCallback<GoUctFeatureCommands>::Method method);

    const GoBoard& m_bd;
    
    FeFeatureWeights m_weights;

    GoUctPlayoutPolicy<GoBoard> m_policy;
};

#endif // FE_COMMANDS_H
