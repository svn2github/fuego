//----------------------------------------------------------------------------
/** @file GoUctCommands.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_COMMANDS_H
#define GOUCT_COMMANDS_H

#include <string>
#include "GtpEngine.h"
#include "GoUctPlayoutPolicy.h"
#include "GoUctGlobalSearch.h"
#include "GoUctPlayer.h"

class GoBoard;
class GoGame;
class GoPlayer;
class GoUctBoard;
class GoUctSearch;

typedef SgUctValue (*MeanMapperFunction)(SgUctValue);

/** Compare current move with engine-produced move types */
enum GoUctCompareMoveType
{
    /**  */
    GOUCT_COMPAREMOVE_CORRECTED,

    /**  */
    GOUCT_COMPAREMOVE_POLICY
};


//----------------------------------------------------------------------------

/** GTP commands for GoUctPlayer.
    Some of the commands are also usable for other players, as long as they
    use a subclass of GoUctSearch (and implement GoUctObjectWithSearch). */
class GoUctCommands
{
public:
    /** Constructor.
        @param bd The game board.
        @param player Reference to pointer to current player, this player can
        be null or a different player, but those commands of this class that
        need a GoUctPlayer will fail, if the current player is not
        GoUctPlayer. */
    GoUctCommands(const GoBoard& bd, GoPlayer*& player, const GoGame& game);

    void AddGoGuiAnalyzeCommands(GtpCommand& cmd);

    /** @page gouctgtpcommands GoUctCommands Commands
        - @link CmdApproximateTerritory() @c approximate_territory @endlink
        - @link CmdFinalScore() @c final_score @endlink
        - @link CmdFinalStatusList() @c final_status_list @endlink
        - @link CmdAdditiveKnowledge() @c uct_additive_knowledge @endlink
        - @link CmdBounds() @c uct_bounds @endlink
        - @link CmdDefaultPolicy() @c uct_default_policy @endlink
        - @link CmdDeterministicMode() @c deterministic_mode @endlink
        - @link CmdEstimatorStat() @c uct_estimator_stat @endlink
        - @link CmdGfx() @c uct_gfx @endlink
        - @link CmdIsPolicyCorrectedMove() @c is_policy_corrected_move
          @endlink
        - @link CmdLadderKnowledge() @c uct_ladder_knowledge @endlink
        - @link CmdMaxMemory() @c uct_max_memory @endlink
        - @link CmdMoves() @c uct_moves @endlink
        - @link CmdNodeInfo() @c uct_node_info @endlink
        - @link CmdParamGlobalSearch() @c uct_param_globalsearch @endlink
        - @link CmdParamPolicy() @c uct_param_policy @endlink
        - @link CmdParamPlayer() @c uct_param_player @endlink
        - @link CmdParamRootFilter() @c uct_param_rootfilter @endlink
        - @link CmdParamSearch() @c uct_param_search @endlink
        - @link CmdParamTreeFilter() @c uct_param_treefilter @endlink
        - @link CmdPatterns() @c uct_patterns @endlink
        - @link CmdPolicyCorrectedMoves() @c uct_policy_corrected_moves 
          @endlink
        - @link CmdPolicyMoves() @c uct_policy_moves @endlink
        - @link CmdPriorKnowledge() @c uct_prior_knowledge @endlink
        - @link CmdRaveValues() @c uct_rave_values @endlink
        - @link CmdRootFilter() @c uct_root_filter @endlink
        - @link CmdSaveGames() @c uct_savegames @endlink
        - @link CmdSaveTree() @c uct_savetree @endlink
        - @link CmdSequence() @c uct_sequence @endlink
        - @link CmdScore() @c uct_score @endlink
        - @link CmdStatPlayer() @c uct_stat_player @endlink
        - @link CmdStatPlayerClear() @c uct_stat_player_clear @endlink
        - @link CmdStatPolicy() @c uct_stat_policy @endlink
        - @link CmdStatPolicyClear() @c uct_stat_policy_clear @endlink
        - @link CmdStatSearch() @c uct_stat_search @endlink
        - @link CmdStatTerritory() @c uct_stat_territory @endlink
        - @link CmdValue() @c uct_value @endlink
        - @link CmdValueBlack() @c uct_value_black @endlink */
    /** @name Command Callbacks */
    // @{
    // The callback functions are documented in the cpp file
    void CmdAdditiveKnowledge(GtpCommand& cmd);
    void CmdApproximateTerritory(GtpCommand& cmd);
    void CmdBounds(GtpCommand& cmd);
    void CmdDefaultPolicy(GtpCommand& cmd);
    void CmdDeterministicMode(GtpCommand&);
    void CmdEstimatorStat(GtpCommand& cmd);
    void CmdFinalScore(GtpCommand&);
    void CmdFinalStatusList(GtpCommand&);
    void CmdGfx(GtpCommand& cmd);
    void CmdIsPolicyCorrectedMove(GtpCommand& cmd);
    void CmdIsPolicyMove(GtpCommand& cmd);
    void CmdLadderKnowledge(GtpCommand& cmd);
    void CmdMaxMemory(GtpCommand& cmd);
    void CmdMoves(GtpCommand& cmd);
    void CmdNodeInfo(GtpCommand& cmd);
    void CmdParamGlobalSearch(GtpCommand& cmd);
    void CmdParamFeatureKnowledge(GtpCommand& cmd);
    void CmdParamPolicy(GtpCommand& cmd);
    void CmdParamPlayer(GtpCommand& cmd);
    void CmdParamRootFilter(GtpCommand& cmd);
    void CmdParamSearch(GtpCommand& cmd);
    void CmdParamTreeFilter(GtpCommand& cmd);
    void CmdPatterns(GtpCommand& cmd);
    void CmdPolicyCorrectedMoves(GtpCommand& cmd);
    void CmdPolicyMoves(GtpCommand& cmd);
    void CmdPriorKnowledge(GtpCommand& cmd);
    void CmdRaveValues(GtpCommand& cmd);
    void CmdRootFilter(GtpCommand& cmd);
    void CmdSaveGames(GtpCommand& cmd);
    void CmdSaveTree(GtpCommand& cmd);
    void CmdScore(GtpCommand& cmd);
    void CmdSequence(GtpCommand& cmd);
    void CmdStatPlayer(GtpCommand& cmd);
    void CmdStatPlayerClear(GtpCommand& cmd);
    void CmdStatPolicy(GtpCommand& cmd);
    void CmdStatPolicyClear(GtpCommand& cmd);
    void CmdStatSearch(GtpCommand& cmd);
    void CmdStatTerritory(GtpCommand& cmd);
    void CmdValue(GtpCommand& cmd);
    void CmdValueBlack(GtpCommand& cmd);
    // @} // @name

    void Register(GtpEngine& engine);

private:
    const GoBoard& m_bd;

    GoPlayer*& m_player;

    const GoGame& m_game;

	/** Check if current move is produced by some engine function.
        Used for verifying filters etc. against professional game records. */
    void CompareMove(GtpCommand& cmd, GoUctCompareMoveType type);

	/** Compute and display prior knowledge.
    	Display either additive knowledge only, or all knowledge */
    void DisplayKnowledge(GtpCommand& cmd, bool additiveKnowledge);

	/** Display contents of SgUctMoveInfo.
    	Either show additive knowledge only, or all knowledge
     */
    void DisplayMoveInfo(GtpCommand& cmd, 
                         const std::vector<SgUctMoveInfo>& moves,
                         bool additiveKnowledge);


    /** Display territory, with mean mapped to territory intensity by f 
        Helper function for @see CmdApproximateTerritory
        and @see CmdStatTerritory */
    SgUctValue DisplayTerritory(GtpCommand& cmd, MeanMapperFunction f);

    SgPointSet DoFinalStatusSearch();

    inline const GoGame& Game();

    GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
                      GoUctPlayoutPolicyFactory<GoUctBoard> >&
        GlobalSearch();

    GoUctPlayer<GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
        GoUctPlayoutPolicyFactory<GoUctBoard> >,
        GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> > >&
        Player();

    GoUctPlayoutPolicy<GoUctBoard>& Policy(unsigned int threadId);

    void Register(GtpEngine& e, const std::string& command,
                  GtpCallback<GoUctCommands>::Method method);

    GoUctSearch& Search();

    GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> >&
    ThreadState(unsigned int threadId);
};

//----------------------------------------------------------------------------

inline const GoGame& GoUctCommands::Game()
{
    return m_game;
}

//----------------------------------------------------------------------------

#endif // GOUCT_COMMANDS_H
