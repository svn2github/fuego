//----------------------------------------------------------------------------
/** @file FeCommands.h
    GTP commands for features. */
//----------------------------------------------------------------------------

#ifndef FE_COMMANDS_H
#define FE_COMMANDS_H

#include <iostream>
#include <string>
#include "GtpEngine.h"

#include "FeBasicFeatures.h"
#include "FeFeatureWeights.h"

class GoBoard;
class GoGame;
class GoPlayer;

/** GTP commands for features.
*/
class FeCommands
{
public:
    /** Constructor.
     @param bd The game board.
     @param player Reference to pointer to current player, this player can
     be null or a different player, but those commands of this class that
     need a GoUctPlayer will fail, if the current player is not
     GoUctPlayer. */
    FeCommands(const GoBoard& bd, GoPlayer*& player, const GoGame& game);

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
                  GtpCallback<FeCommands>::Method method);

    const GoBoard& m_bd;

    GoPlayer*& m_player;

    const GoGame& m_game;
    
    FeFeatureWeights m_weights;
};

#endif // FE_COMMANDS_H
