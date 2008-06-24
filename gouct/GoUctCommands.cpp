//----------------------------------------------------------------------------
/** @file GoUctCommands.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctCommands.h"

#include <fstream>
#include <boost/format.hpp>
#include "GoEyeUtil.h"
#include "GoGtpCommandUtil.h"
#include "GoBoardUtil.h"
#include "GoUctDefaultPlayoutPolicy.h"
#include "GoUctEstimatorStat.h"
#include "GoUctGlobalSearchPlayer.h"
#include "GoUctGlobalSearch.h"
#include "GoUctPatterns.h"
#include "GoUctUtil.h"
#include "SgException.h"
#include "SgUctTreeUtil.h"
#include "SgWrite.h"

using namespace std;
using boost::format;
using GoGtpCommandUtil::BlackWhiteArg;
using GoGtpCommandUtil::EmptyPointArg;
using GoGtpCommandUtil::PointArg;

//----------------------------------------------------------------------------

namespace {

GoUctLiveGfxMode LiveGfxArg(const GtpCommand& cmd, size_t number)
{
    string arg = cmd.ArgToLower(number);
    if (arg == "none")
        return GOUCT_LIVEGFXMODE_NONE;
    if (arg == "counts")
        return GOUCT_LIVEGFXMODE_COUNTS;
    if (arg == "sequence")
        return GOUCT_LIVEGFXMODE_SEQUENCE;
    throw GtpFailure() << "unknown live-gfx argument \"" << arg << '"';
}

string LiveGfxToString(GoUctLiveGfxMode mode)
{
    switch (mode)
    {
    case GOUCT_LIVEGFXMODE_NONE:
        return "none";
    case GOUCT_LIVEGFXMODE_COUNTS:
        return "counts";
    case GOUCT_LIVEGFXMODE_SEQUENCE:
        return "sequence";
    default:
        SG_ASSERT(false);
        return "?";
    }
}

SgUctMoveSelect MoveSelectArg(const GtpCommand& cmd, size_t number)
{
    string arg = cmd.ArgToLower(number);
    if (arg == "value")
        return SG_UCTMOVESELECT_VALUE;
    if (arg == "count")
        return SG_UCTMOVESELECT_COUNT;
    if (arg == "bound")
        return SG_UCTMOVESELECT_BOUND;
    if (arg == "estimate")
        return SG_UCTMOVESELECT_ESTIMATE;
    throw GtpFailure() << "unknown move select argument \"" << arg << '"';
}

string MoveSelectToString(SgUctMoveSelect moveSelect)
{
    switch (moveSelect)
    {
    case SG_UCTMOVESELECT_VALUE:
        return "value";
    case SG_UCTMOVESELECT_COUNT:
        return "count";
    case SG_UCTMOVESELECT_BOUND:
        return "bound";
    case SG_UCTMOVESELECT_ESTIMATE:
        return "estimate";
    default:
        SG_ASSERT(false);
        return "?";
    }
}

SgUctPriorInit PriorInitArg(const GtpCommand& cmd, size_t number)
{
    string arg = cmd.ArgToLower(number);
    if (arg == "move")
        return SG_UCTPRIORINIT_MOVE;
    if (arg == "rave")
        return SG_UCTPRIORINIT_RAVE;
    if (arg == "both")
        return SG_UCTPRIORINIT_BOTH;
    throw GtpFailure() << "unknown prior init argument \"" << arg << '"';
}

string PriorInitToString(SgUctPriorInit priorInit)
{
    switch (priorInit)
    {
    case SG_UCTPRIORINIT_MOVE:
        return "move";
    case SG_UCTPRIORINIT_RAVE:
        return "rave";
    case SG_UCTPRIORINIT_BOTH:
        return "both";
    default:
        SG_ASSERT(false);
        return "?";
    }
}

GoUctGlobalSearchPrior PriorKnowledgeArg(const GtpCommand& cmd, size_t number)
{
    string arg = cmd.ArgToLower(number);
    if (arg == "none")
        return GOUCT_PRIORKNOWLEDGE_NONE;
    if (arg == "even")
        return GOUCT_PRIORKNOWLEDGE_EVEN;
    if (arg == "default")
        return GOUCT_PRIORKNOWLEDGE_DEFAULT;
    throw GtpFailure() << "unknown prior knowledge argument \"" << arg << '"';
}

string PriorKnowledgeToString(GoUctGlobalSearchPrior prior)
{
    switch (prior)
    {
    case GOUCT_PRIORKNOWLEDGE_NONE:
        return "none";
    case GOUCT_PRIORKNOWLEDGE_EVEN:
        return "even";
    case GOUCT_PRIORKNOWLEDGE_DEFAULT:
        return "default";
    default:
        SG_ASSERT(false);
        return "?";
    }
}

GoUctGlobalSearchMode SearchModeArg(const GtpCommand& cmd, size_t number)
{
    string arg = cmd.ArgToLower(number);
    if (arg == "playout_policy")
        return GOUCT_SEARCHMODE_PLAYOUTPOLICY;
    if (arg == "uct")
        return GOUCT_SEARCHMODE_UCT;
    if (arg == "one_ply")
        return GOUCT_SEARCHMODE_ONEPLY;
    throw GtpFailure() << "unknown search mode argument \"" << arg << '"';
}

string SearchModeToString(GoUctGlobalSearchMode mode)
{
    switch (mode)
    {
    case GOUCT_SEARCHMODE_PLAYOUTPOLICY:
        return "playout_policy";
    case GOUCT_SEARCHMODE_UCT:
        return "uct";
    case GOUCT_SEARCHMODE_ONEPLY:
        return "one_ply";
    default:
        SG_ASSERT(false);
        return "?";
    }
}

} // namespace

//----------------------------------------------------------------------------

GoUctCommands::GoUctCommands(GoBoard& bd, GoPlayer*& player)
    : m_bd(bd),
      m_player(player)
{
}

void GoUctCommands::AddGoGuiAnalyzeCommands(GtpCommand& cmd)
{
    cmd <<
        "gfx/Uct Bounds/uct_bounds\n"
        "gfx/Uct Gfx/uct_gfx\n"
        "plist/Uct Moves/uct_moves\n"
        "param/Uct Param GlobalSearch/uct_param_globalsearch\n"
        "param/Uct Param Policy/uct_param_policy\n"
        "param/Uct Param Player/uct_param_player\n"
        "param/Uct Param Search/uct_param_search\n"
        "plist/Uct Patterns/uct_patterns\n"
        "pstring/Uct Policy Moves/uct_policy_moves\n"
        "sboard/Uct Rave Values/uct_rave_values\n"
        "plist/Uct Root Filter/uct_root_filter\n"
        "none/Uct SaveGames/uct_savegames %w\n"
        "none/Uct SaveTree/uct_savetree %w\n"
        "gfx/Uct Sequence/uct_sequence\n"
        "sboard/Uct Signature Code/uct_signature_code\n"
        "sboard/Uct Signature Value/uct_signature_value\n"
        "sboard/Uct Signature Count/uct_signature_count\n"
        "hstring/Uct Stat Player/uct_stat_player\n"
        "none/Uct Stat Player Clear/uct_stat_player_clear\n"
        "hstring/Uct Stat Policy/uct_stat_policy\n"
        "none/Uct Stat Policy Clear/uct_stat_policy_clear\n"
        "hstring/Uct Stat Search/uct_stat_search\n";
}

/** Show UCT bounds of moves in root node.
    This command is compatible with the GoGui analyze command type "gfx".
    Move bounds are shown as labels on the board, the pass move bound is
    shown as text in the status line.
    @see SgUctSearch::GetBound
*/
void GoUctCommands::CmdBounds(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    const GoUctSearch& search = Search();
    const SgUctTree& tree = search.Tree();
    const SgUctNode& root = tree.Root();
    bool hasPass = false;
    float passBound = 0;
    cmd << "LABEL";
    for (SgUctChildIterator it(tree, root); it; ++it)
    {
        const SgUctNode& child = *it;
        SgPoint move = child.Move();
        float bound = search.GetBound(root, child);
        if (move == SG_PASS)
        {
            hasPass = true;
            passBound = bound;
        }
        else
            cmd << ' ' << SgWritePoint(move) << ' ' << fixed
                << setprecision(2) << bound;
    }
    cmd << '\n';
    if (hasPass)
        cmd << "TEXT PASS=" << fixed << setprecision(2) << passBound << '\n';
}

/** Compute estimator statistics.
    Arguments: trueValueMaxGames maxGames stepSize fileName
    @see GoUctEstimatorStat::Compute()
*/
void GoUctCommands::CmdEstimatorStat(GtpCommand& cmd)
{
    cmd.CheckNuArg(4);
    size_t trueValueMaxGames = cmd.SizeTypeArg(0);
    size_t maxGames = cmd.SizeTypeArg(1);
    size_t stepSize = cmd.SizeTypeArg(2);
    string fileName = cmd.Arg(3);
    GoUctEstimatorStat::Compute(Search(), trueValueMaxGames, maxGames,
                                stepSize, fileName);
}

/** Show move values and sample numbers of last search.
    Arguments: none
    @see GoUctSearch::GoGuiGfx()
*/
void GoUctCommands::CmdGfx(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    GoUctUtil::GoGuiGfx(Search(), Search().ToPlay(), cmd);
}

/** Return a list of all moves that the search would generate in the current
    position.
    Arguments: none
    @see SgUctSearch::GenerateAllMoves()
*/
void GoUctCommands::CmdMoves(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    vector<SgPoint> moves;
    Search().GenerateAllMoves(moves);
    cmd << SgWritePointList(moves, "", false);
}

/** Get and set GoUctGlobalSearch parameters.
    This command is compatible with the GoGui analyze command type "param".

    Parameters:
    @arg @c score_modification See
        GoUctGlobalSearchStateParam::m_scoreModification
    @arg @c mercy_rule See GoUctGlobalSearchStateParam::m_mercyRule
*/
void GoUctCommands::CmdParamGlobalSearch(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    GoUctGlobalSearchStateParam& p = GlobalSearch().m_param;
    if (cmd.NuArg() == 0)
    {
        // Boolean parameters first for better layout of GoGui parameter
        // dialog, alphabetically otherwise
        cmd << "[bool] mercy_rule " << p.m_mercyRule << '\n'
            << "[string] score_modification " << p.m_scoreModification
            << '\n';
    }
    else if (cmd.NuArg() == 2)
    {
        string name = cmd.Arg(0);
        if (name == "mercy_rule")
            p.m_mercyRule = cmd.BoolArg(1);
        else if (name == "score_modification")
            p.m_scoreModification = cmd.FloatArg(1);
        else
            throw GtpFailure() << "unknown parameter: " << name;
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

/** Get and set GoUctGlobalSearchPlayer parameters.
    This command is compatible with the GoGui analyze command type "param".

    Parameters:
    @arg @c auto_param See GoUctGlobalSearchPlayer::AutoParam
    @arg @c ignore_clock See GoUctGlobalSearchPlayer::IgnoreClock
    @arg @c reuse_subtree See GoUctGlobalSearchPlayer::ReuseSubtree
    @arg @c max_games See GoUctGlobalSearchPlayer::MaxGames
    @arg @c max_nodes See GoUctGlobalSearchPlayer::MaxNodes
    @arg @c max_time See GoUctGlobalSearchPlayer::MaxTime
    @arg @c monitor_stat_file See GoUctGlobalSearchPlayer::MonitorStatFile
    @arg @c prior_knowledge @c none|even|policy See
      GoUctGlobalSearchPlayer::PriorKnowledge
    @arg @c resign_threshold See GoUctGlobalSearchPlayer::ResignThreshold
    @arg @c search_mode @c playout|uct|one_ply See
      GoUctGlobalSearchPlayer::SearchMode
*/
void GoUctCommands::CmdParamPlayer(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    GoUctGlobalSearchPlayer& p = Player();
    if (cmd.NuArg() == 0)
    {
        // Boolean parameters first for better layout of GoGui parameter
        // dialog, alphabetically otherwise
        cmd << "[bool] auto_param " << p.AutoParam() << '\n'
            << "[bool] ignore_clock " << p.IgnoreClock() << '\n'
            << "[bool] ponder " << p.EnablePonder() << '\n'
            << "[bool] prune_root_moves " << p.PruneRootMoves() << '\n'
            << "[bool] reuse_subtree " << p.ReuseSubtree() << '\n'
            << "[string] max_games " << p.MaxGames() << '\n'
            << "[string] max_nodes " << p.MaxNodes() << '\n'
            << "[string] max_time " << p.MaxTime() << '\n'
            << "[string] monitor_stat_file " << p.MonitorStatFile() << '\n'
            << "[list/none/even/default] prior_knowledge "
            << PriorKnowledgeToString(p.PriorKnowledge()) << '\n'
            << "[string] resign_threshold " << p.ResignThreshold() << '\n'
            << "[list/playout_policy/uct/one_ply] search_mode "
            << SearchModeToString(p.SearchMode()) << '\n';
    }
    else if (cmd.NuArg() >= 1 && cmd.NuArg() <= 2)
    {
        string name = cmd.Arg(0);
        if (name == "auto_param")
            p.SetAutoParam(cmd.BoolArg(1));
        else if (name == "ignore_clock")
            p.SetIgnoreClock(cmd.BoolArg(1));
        else if (name == "ponder")
            p.SetEnablePonder(cmd.BoolArg(1));
        else if (name == "prune_root_moves")
            p.SetPruneRootMoves(cmd.BoolArg(1));
        else if (name == "reuse_subtree")
            p.SetReuseSubtree(cmd.BoolArg(1));
        else if (name == "max_games")
            p.SetMaxGames(cmd.SizeTypeArg(1, 1));
        else if (name == "max_nodes")
            p.SetMaxNodes(cmd.SizeTypeArg(1, 1));
        else if (name == "max_time")
            p.SetMaxTime(cmd.FloatArg(1));
        else if (name == "monitor_stat_file")
            p.SetMonitorStatFile(cmd.RemainingLine(0));
        else if (name == "prior_knowledge")
            p.SetPriorKnowledge(PriorKnowledgeArg(cmd, 1));
        else if (name == "resign_threshold")
            p.SetResignThreshold(cmd.FloatArg(1));
        else if (name == "search_mode")
            p.SetSearchMode(SearchModeArg(cmd, 1));
        else
            throw GtpFailure() << "unknown parameter: " << name;
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

/** Get and set GoUctDefaultPlayoutPolicy parameters.
    This command is compatible with the GoGui analyze command type "param".

    Parameters:
    @arg @c pure_random See GoUctDefaultPlayoutPolicyParam::m_pureRandom
    @arg @c clump_correction
      See GoUctDefaultPlayoutPolicyParam::m_useClumpCorrection
    @arg @c statistics_enables
      See GoUctDefaultPlayoutPolicyParam::m_statisticsEnabled
*/
void GoUctCommands::CmdParamPolicy(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    GoUctDefaultPlayoutPolicyParam& p = Player().m_playoutPolicyParam;
    if (cmd.NuArg() == 0)
    {
        // Boolean parameters first for better layout of GoGui parameter
        // dialog, alphabetically otherwise
        cmd << "[bool] pure_random " << p.m_pureRandom << '\n'
            << "[bool] clump_correction " << p.m_useClumpCorrection << '\n'
            << "[bool] statistics_enabled " << p.m_statisticsEnabled << '\n';
    }
    else if (cmd.NuArg() == 2)
    {
        string name = cmd.Arg(0);
        if (name == "pure_random")
            p.m_pureRandom = cmd.BoolArg(1);
        else if (name == "clump_correction")
            p.m_useClumpCorrection = cmd.BoolArg(1);
        else if (name == "statistics_enabled")
            p.m_statisticsEnabled = cmd.BoolArg(1);
        else
            throw GtpFailure() << "unknown parameter: " << name;
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

/** Get and set SgUctSearch and GoUctSearch parameters.
    This command is compatible with the GoGui analyze command type "param".

    Parameters:
    @arg @c keep_games See GoUctSearch::KeepGames
    @arg @c lock_free See SgUctSearch::LockFree
    @arg @c log_games See SgUctSearch::LogGames
    @arg @c no_bias_term See SgUctSearch::NoBiasTerm
    @arg @c rave See SgUctSearch::Rave
    @arg @c rave_check_same SgUctSearch::RaveCheckSame
    @arg @c use_signatures SgUctSearch::UseSignatures
    @arg @c bias_term_constant See SgUctSearch::BiasTermConstant
    @arg @c expand_threshold See SgUctSearch::ExpandThreshold
    @arg @c first_play_urgency See SgUctSearch::FirstPlayUrgency
    @arg @c live_gfx @c none|counts|sequence See GoUctSearch::LiveGfx
    @arg @c live_gfx_interval See GoUctSearch::LiveGfxInterval
    @arg @c move_select @c value|count|bound|rave See SgUctSearch::MoveSelect
    @arg @c number_threads See SgUctSearch::NumberThreads
    @arg @c number_playouts See SgUctSearch::NumberPlayouts
    @arg @c prior_init @c move|rave|both See SgUctSearch::PriorInit
    @arg @c rave_weight_final See SgUctSearch::RaveWeightFinal
    @arg @c rave_weight_initial See SgUctSearch::RaveWeightInitial
    @arg @c signature_weight_final See SgUctSearch::SignatureWeightFinal
    @arg @c signature_weight_initial See SgUctSearch::SignatureWeightInitial
*/
void GoUctCommands::CmdParamSearch(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    GoUctSearch& s = Search();
    if (cmd.NuArg() == 0)
    {
        // Boolean parameters first for better layout of GoGui parameter
        // dialog, alphabetically otherwise
        cmd << "[bool] keep_games " << s.KeepGames() << '\n'
            << "[bool] lock_free " << s.LockFree() << '\n'
            << "[bool] log_games " << s.LogGames() << '\n'
            << "[bool] no_bias_term " << s.NoBiasTerm() << '\n'
            << "[bool] rave " << s.Rave() << '\n'
            << "[bool] rave_check_same " << s.RaveCheckSame() << '\n'
            << "[bool] use_signatures " << s.UseSignatures() << '\n'
            << "[string] bias_term_constant " << s.BiasTermConstant() << '\n'
            << "[string] expand_threshold " << s.ExpandThreshold() << '\n'
            << "[string] first_play_urgency " << s.FirstPlayUrgency() << '\n'
            << "[list/none/counts/sequence] live_gfx "
            << LiveGfxToString(s.LiveGfx()) << '\n'
            << "[string] live_gfx_interval " << s.LiveGfxInterval() << '\n'
            << "[list/value/count/bound/estimate] move_select "
            << MoveSelectToString(s.MoveSelect()) << '\n'
            << "[string] number_threads " << s.NumberThreads() << '\n'
            << "[string] number_playouts " << s.NumberPlayouts() << '\n'
            << "[list/move/rave/both] prior_init "
            << PriorInitToString(s.PriorInit()) << '\n'
            << "[string] rave_weight_final " << s.RaveWeightFinal() << '\n'
            << "[string] rave_weight_initial "
            << s.RaveWeightInitial() << '\n'
            << "[string] signature_weight_final "
            << s.SignatureWeightFinal() << '\n'
            << "[string] signature_weight_initial "
            << s.SignatureWeightInitial() << '\n';

    }
    else if (cmd.NuArg() == 2)
    {
        string name = cmd.Arg(0);
        if (name == "keep_games")
            s.SetKeepGames(cmd.BoolArg(1));
        else if (name == "lock_free")
            s.SetLockFree(cmd.BoolArg(1));
        else if (name == "log_games")
            s.SetLogGames(cmd.BoolArg(1));
        else if (name == "no_bias_term")
            s.SetNoBiasTerm(cmd.BoolArg(1));
        else if (name == "rave")
            s.SetRave(cmd.BoolArg(1));
        else if (name == "rave_check_same")
            s.SetRaveCheckSame(cmd.BoolArg(1));
        else if (name == "use_signatures")
            s.SetUseSignatures(cmd.BoolArg(1));
        else if (name == "bias_term_constant")
            s.SetBiasTermConstant(cmd.FloatArg(1));
        else if (name == "expand_threshold")
            s.SetExpandThreshold(cmd.SizeTypeArg(1, 1));
        else if (name == "first_play_urgency")
            s.SetFirstPlayUrgency(cmd.FloatArg(1));
        else if (name == "live_gfx")
            s.SetLiveGfx(LiveGfxArg(cmd, 1));
        else if (name == "live_gfx_interval")
            s.SetLiveGfxInterval(cmd.IntArg(1, 1));
        else if (name == "move_select")
            s.SetMoveSelect(MoveSelectArg(cmd, 1));
        else if (name == "number_threads")
            s.SetNumberThreads(cmd.SizeTypeArg(1, 1));
        else if (name == "number_playouts")
            s.SetNumberPlayouts(cmd.IntArg(1, 1));
        else if (name == "prior_init")
            s.SetPriorInit(PriorInitArg(cmd, 1));
        else if (name == "rave_weight_final")
            s.SetRaveWeightFinal(cmd.FloatArg(1));
        else if (name == "rave_weight_initial")
            s.SetRaveWeightInitial(cmd.FloatArg(1));
        else if (name == "signature_weight_final")
            s.SetSignatureWeightFinal(cmd.FloatArg(1));
        else if (name == "signature_weight_initial")
            s.SetSignatureWeightInitial(cmd.FloatArg(1));
        else
            throw GtpFailure() << "unknown parameter: " << name;
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

/** Show matching patterns.
    Returns: List of points for matching patterns
    @see GoUctPatterns
*/
void GoUctCommands::CmdPatterns(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    GoUctPatterns<GoBoard> patterns(m_bd);
    for (GoBoard::Iterator it(m_bd); it; ++it)
        if (m_bd.IsEmpty(*it) && patterns.MatchAny(*it))
            cmd << SgWritePoint(*it) << ' ';
}

/** Return equivalent best moves in playout policy.
    See GoUctDefaultPlayoutPolicy::GetEquivalentBestMoves() <br>
    Arguments: none <br>
    Returns: Move type string followed by move list on a single line.
*/
void GoUctCommands::CmdPolicyMoves(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    SgBWSet safe; // Not used
    SgPointArray<bool> allSafe(false); // Not used
    GoUctDefaultPlayoutPolicy<GoBoard> policy(m_bd,
                                              Player().m_playoutPolicyParam,
                                              safe, allSafe);
    policy.StartPlayout();
    policy.GenerateMove();
    cmd << GoUctDefaultPlayoutPolicyTypeStr(policy.MoveType());
    GoPointList moves = policy.GetEquivalentBestMoves();
    for (int i = 0; i < moves.Length(); ++i)
        cmd << ' ' << SgWritePoint(moves[i]);
}

/** Show RAVE values of last search at root position.
    This command is compatible to the GoGui analyze command type @c dboard.
    The values are scaled to [-1,+1] from Black's point of view.
    @see SgUctSearch::Rave
*/
void GoUctCommands::CmdRaveValues(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    const GoUctSearch& search = Search();
    if (! search.Rave())
        throw GtpFailure("RAVE not enabled");
    SgPointArray<string> array("\"\"");
    const SgUctTree& tree = search.Tree();
    for (SgUctChildIterator it(tree, tree.Root()); it; ++it)
    {
        const SgUctNode& child = *it;
        SgPoint p = child.Move();
        if (p == SG_PASS || child.RaveCount() == 0)
            continue;
        ostringstream out;
        out << fixed << setprecision(2) << child.RaveValue();
        array[p] = out.str();
    }
    cmd << '\n'
        << SgWritePointArray<string>(array, m_bd.Size());
}

/** Return filtered root moves.
    @see GoUctRootFilter::Get()
*/
void GoUctCommands::CmdRootFilter(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    cmd << SgWritePointList(Player().RootFilter().Get(), "", false);
}

/** Save the UCT tree in SGF format.
    Arguments: filename
    @see GoUctSearch::SaveTree()
*/
void GoUctCommands::CmdSaveTree(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    string fileName = cmd.Arg(0);
    ofstream out(fileName.c_str());
    if (! out)
        throw GtpFailure() << "Could not open " << fileName;
    Search().SaveTree(out);
}

/** Save all random games.
    Arguments: filename
    @see GoUctSearch::SaveGames()
*/
void GoUctCommands::CmdSaveGames(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    string fileName = cmd.Arg(0);
    try
    {
        Search().SaveGames(fileName);
    }
    catch (const SgException& e)
    {
        throw GtpFailure(e.what());
    }
}

/** Count the score using the scoring function of UCT.
    Arguments: none <br>
    Returns: Score (Win/Loss)
    @see GoBoardUtil::ScoreSimpleEndPosition()
*/
void GoUctCommands::CmdScore(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    try
    {
        float komi = m_bd.Rules().Komi().ToFloat();
        cmd << GoBoardUtil::ScoreSimpleEndPosition(m_bd, komi);
    }
    catch (const SgException& e)
    {
        throw GtpFailure(e.what());
    }
}

/** Show the best sequence from last search.
    This command is compatible with the GoGui analyze command type "gfx"
    (There is no "var" command type supported in GoGui 1.1pre1, which allows
    to specify the first color to move within the response, and this command
    returns the sequence of the last search, which is unrelated to the current
    color to play on the board.) <br>
    Arguments: none
*/
void GoUctCommands::CmdSequence(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    GoUctUtil::PrintBestSequence(Search(), Search().ToPlay(), cmd);
}

/** Show signature codes.
    This command is compatible with the GoGui analyze command type "sboard"
*/
void GoUctCommands::CmdSignatureCode(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    const GoUctSearch& search = Search();
    SgPointArray<string> array("\"\"");
    for (GoBoard::Iterator it(m_bd); it; ++it)
    {
        SgPoint p = *it;
        if (m_bd.Occupied(p))
            continue;
        size_t sig = search.GetSignature(p);
        if (sig != numeric_limits<size_t>::max())
            array[p] = str(format("%d") % sig);
    }
    cmd << '\n' << SgWritePointArray<string>(array, m_bd.Size());

}

/** Show signature counts.
    This command is compatible with the GoGui analyze command type "sboard"
*/
void GoUctCommands::CmdSignatureCount(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    const GoUctSearch& search = Search();
    SgPointArray<string> array("\"\"");
    for (GoBoard::Iterator it(m_bd); it; ++it)
    {
        SgPoint p = *it;
        if (m_bd.Occupied(p))
            continue;
        size_t sig = search.GetSignature(p);
        if (sig != numeric_limits<size_t>::max())
        {
            try
            {
                size_t count = search.GetSignatureStat(sig).Count();
                array[p] = str(format("%d") % count);
            }
            catch (const SgException& e)
            {
                throw GtpFailure(e.what());
            }
        }
    }
    cmd << '\n' << SgWritePointArray<string>(array, m_bd.Size());

}

/** Show signature values.
    This command is compatible with the GoGui analyze command type "sboard"
*/
void GoUctCommands::CmdSignatureValue(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    const GoUctSearch& search = Search();
    SgPointArray<string> array("\"\"");
    for (GoBoard::Iterator it(m_bd); it; ++it)
    {
        SgPoint p = *it;
        if (m_bd.Occupied(p))
            continue;
        size_t sig = search.GetSignature(p);
        if (sig != numeric_limits<size_t>::max())
        {
            try
            {
                if (search.GetSignatureStat(sig).Count() > 0)
                {
                    float mean = search.GetSignatureStat(sig).Mean();
                    array[p] = str(format("%.2f") % mean);
                }
            }
            catch (const SgException& e)
            {
                throw GtpFailure(e.what());
            }
        }
    }
    cmd << '\n' << SgWritePointArray<string>(array, m_bd.Size());

}

/** Write statistics of GoUctGlobalSearchPlayer.
    Arguments: none
    @see GoUctGlobalSearchPlayer::Statistics
*/
void GoUctCommands::CmdStatPlayer(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    Player().GetStatistics().Write(cmd);
}

/** Clear statistics of GoUctGlobalSearchPlayer.
    Arguments: none
    @see GoUctGlobalSearchPlayer::Statistics
*/
void GoUctCommands::CmdStatPlayerClear(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    Player().ClearStatistics();
}

/** Write statistics of playout policy.
    Arguments: none <br>
    Needs enabling the statistics with
    <code>uct_param_policy statistics_enabled</code>
    Only the statistics of the first thread's policy used.
    @see GoUctDefaultPlayoutPolicyStat
*/
void GoUctCommands::CmdStatPolicy(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    if (! Player().m_playoutPolicyParam.m_statisticsEnabled)
        SgWarning() << "statistics not enabled in policy parameters\n";
    Policy(0).Statistics().Write(cmd);
}

/** Clear statistics of GoUctDefaultPlayoutPolicy
    Arguments: none <br>
    Only the statistics of the first thread's policy used.
    @see GoUctDefaultPlayoutPolicyStat
*/
void GoUctCommands::CmdStatPolicyClear(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    Policy(0).ClearStatistics();
}

/** Write statistics of search and tree.
    Arguments: none
    @see SgUctSearch::WriteStatistics()
*/
void GoUctCommands::CmdStatSearch(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    const GoUctSearch& search = Search();
    SgUctTreeStatistics treeStatistics;
    treeStatistics.Compute(search.Tree(), search);
    cmd << "SearchStatistics:\n";
    search.WriteStatistics(cmd);
    cmd << "TreeStatistics:\n"
        << treeStatistics;
}

/** Return value of root node from last search.
    Arguments: none
*/
void GoUctCommands::CmdValue(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    cmd << Search().Tree().Root().Mean();
}

GoUctGlobalSearch& GoUctCommands::GlobalSearch()
{
    return Player().GlobalSearch();
}

GoUctGlobalSearchPlayer& GoUctCommands::Player()
{
    if (m_player == 0)
        throw GtpFailure("player not GoUctGlobalSearchPlayer");
    try
    {
        return dynamic_cast<GoUctGlobalSearchPlayer&>(*m_player);
    }
    catch (const bad_cast& e)
    {
        throw GtpFailure("player not GoUctGlobalSearchPlayer");
    }
}

GoUctDefaultPlayoutPolicy<GoUctBoard>&
GoUctCommands::Policy(std::size_t threadId)
{
    GoUctSearch& search = Search();
    if (! search.ThreadsCreated())
        throw GtpFailure("threads not yet created");
    try
    {
        GoUctGlobalSearchState& state =
            dynamic_cast<GoUctGlobalSearchState&>(
                                                search.ThreadState(threadId));
        GoUctDefaultPlayoutPolicy<GoUctBoard>* policy =
            dynamic_cast<GoUctDefaultPlayoutPolicy<GoUctBoard>*>(
                                                              state.Policy());
        if (policy == 0)
            throw GtpFailure("player has no GoUctDefaultPlayoutPolicy");
        return *policy;
    }
    catch (const bad_cast& e)
    {
        throw GtpFailure("player has no GoUctGlobalSearchState");
    }
}

void GoUctCommands::Register(GtpEngine& e)
{
    Register(e, "uct_bounds", &GoUctCommands::CmdBounds);
    Register(e, "uct_estimator_stat", &GoUctCommands::CmdEstimatorStat);
    Register(e, "uct_gfx", &GoUctCommands::CmdGfx);
    Register(e, "uct_moves", &GoUctCommands::CmdMoves);
    Register(e, "uct_param_globalsearch",
             &GoUctCommands::CmdParamGlobalSearch);
    Register(e, "uct_param_policy", &GoUctCommands::CmdParamPolicy);
    Register(e, "uct_param_player", &GoUctCommands::CmdParamPlayer);
    Register(e, "uct_param_search", &GoUctCommands::CmdParamSearch);
    Register(e, "uct_patterns", &GoUctCommands::CmdPatterns);
    Register(e, "uct_policy_moves", &GoUctCommands::CmdPolicyMoves);
    Register(e, "uct_rave_values", &GoUctCommands::CmdRaveValues);
    Register(e, "uct_root_filter", &GoUctCommands::CmdRootFilter);
    Register(e, "uct_savegames", &GoUctCommands::CmdSaveGames);
    Register(e, "uct_savetree", &GoUctCommands::CmdSaveTree);
    Register(e, "uct_sequence", &GoUctCommands::CmdSequence);
    Register(e, "uct_score", &GoUctCommands::CmdScore);
    Register(e, "uct_signature_code", &GoUctCommands::CmdSignatureCode);
    Register(e, "uct_signature_count", &GoUctCommands::CmdSignatureCount);
    Register(e, "uct_signature_value", &GoUctCommands::CmdSignatureValue);
    Register(e, "uct_stat_player", &GoUctCommands::CmdStatPlayer);
    Register(e, "uct_stat_player_clear", &GoUctCommands::CmdStatPlayerClear);
    Register(e, "uct_stat_policy", &GoUctCommands::CmdStatPolicy);
    Register(e, "uct_stat_policy_clear", &GoUctCommands::CmdStatPolicyClear);
    Register(e, "uct_stat_search", &GoUctCommands::CmdStatSearch);
    Register(e, "uct_value", &GoUctCommands::CmdValue);
}

void GoUctCommands::Register(GtpEngine& engine, const std::string& command,
                             GtpCallback<GoUctCommands>::Method method)
{
    engine.Register(command, new GtpCallback<GoUctCommands>(this, method));
}

GoUctSearch& GoUctCommands::Search()
{
    try
    {
        GoUctObjectWithSearch& object =
            dynamic_cast<GoUctObjectWithSearch&>(*m_player);
        return object.Search();
    }
    catch (const bad_cast& e)
    {
        throw GtpFailure("player is not a GoUctObjectWithSearch");
    }
}
//----------------------------------------------------------------------------
