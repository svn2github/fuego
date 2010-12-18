//----------------------------------------------------------------------------
/** @file GoGame.cpp
    See GoGame.h */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoGame.h"

#include "GoBoardUtil.h"
#include "GoInit.h"
#include "GoPlayer.h"
#include "SgNode.h"
#include "SgNodeUtil.h"
#include "SgProp.h"
#include "SgSearchStatistics.h"
#include "SgUtil.h"

using namespace std;
using GoBoardUtil::PlayIfLegal;
using SgUtil::ForceInRange;

//----------------------------------------------------------------------------

namespace {

void AddStatisticsToNode(const SgSearchStatistics* stat, SgNode* node)
{
    node->Add(new SgPropInt(SG_PROP_NUM_NODES, stat->NumNodes()));
    node->Add(new SgPropInt(SG_PROP_NUM_LEAFS, stat->NumEvals()));
    // AR: moves, pass moves
    node->Add(new SgPropMSec(SG_PROP_TIME_USED, stat->TimeUsed()));
    node->Add(new SgPropInt(SG_PROP_MAX_DEPTH, stat->DepthReached()));
}

/** Add up to 4 handicap stones to '*stones', and reduce '*handicap'
    by that amount. */
void AddHandicap(int size, int row, int col, int* handicap,
                 SgVector<SgPoint>* stones)
{
    SG_ASSERT(2 <= *handicap);
    stones->PushBack(SgPointUtil::Pt(size + 1 - col, row));
    stones->PushBack(SgPointUtil::Pt(col, size + 1 - row));
    if (2 < *handicap)
        stones->PushBack(SgPointUtil::Pt(row, col));
    if (3 < *handicap)
        stones->PushBack(SgPointUtil::Pt(size + 1 - row, size + 1 - col));
    if (*handicap < 4)
        *handicap = 0;
    else
        *handicap -= 4;
}

} // namespace

//----------------------------------------------------------------------------

GoGame::GoGame(int boardSize)
    : m_board(boardSize),
      m_current(0),
      m_time(),
      m_oldCommentNode(0),
      m_numMovesToInsert(0)
{
    // Make sure GoInit was called to avoid silent failure of ExecuteMove
    // because of unregistered move property
    GoInitCheck();
    Init(0);
}

GoGame::~GoGame()
{
    m_current->Root()->DeleteTree();
}


SgNode* GoGame::AddMove(SgMove move, SgBlackWhite player,
                        const SgSearchStatistics* stat)
{
    // Check whether a node with that move already exists.
    SgNode* node = m_current->LeftMostSon();
    while (node)
    {
        SgPropMove* prop = static_cast<SgPropMove*>(node->Get(SG_PROP_MOVE));
        if (prop && prop->IsValue(move) && prop->IsPlayer(player))
            break;
        node = node->RightBrother();
    }

    // If no such node exists, create a new node with the given move.
    if (! node)
    {
        if (m_current->HasSon() && 0 < m_numMovesToInsert)
        {
            node = m_current->LeftMostSon()->NewFather();
            --m_numMovesToInsert;
        }
        else
        {
            node = m_current->NewRightMostSon();
            m_numMovesToInsert = 0;
        }
        node->AddMoveProp(move, player);
    }
    // Add statistics and time left to the node.
    if (stat)
        AddStatisticsToNode(stat, node);
    m_time.PlayedMove(*node, player);
    return node;
}

SgNode* GoGame::AddResignNode(SgBlackWhite player)
{
    SgNode* node = m_current->NewRightMostSon();
    ostringstream comment;
    comment << (player == SG_BLACK ? "Black" : "White") << " resigned";
    node->AddComment(comment.str());
    return node;
}

bool GoGame::CanGoInDirection(SgNode::Direction dir) const
{
    SgNode* node = m_current->NodeInDirection(dir);
    return node && node != m_current;
}

SgMove GoGame::CurrentMove() const
{
    const SgNode* node = CurrentNode();
    if (node)
    {
        // Get the move from the property.
        SgPropMove* prop = static_cast<SgPropMove*>(node->Get(SG_PROP_MOVE));
        if (prop)
            return prop->Value();
    }
    return SG_NULLMOVE;
}

int GoGame::CurrentMoveNumber() const
{
    // TODO: once the transition of GoBoard to only support setup stones
    // in the initial position is finished, it will be more efficient to
    // call m_board.MoveNumber() instead of SgNodeUtil::GetMoveNumber()
    return SgNodeUtil::GetMoveNumber(m_current);
}

void GoGame::DeleteTreeAndInitState()
{
    if (m_current)
        m_current->Root()->DeleteTree();
    m_current = 0;
    m_oldCommentNode = 0;
}

bool GoGame::EndOfGame() const
{
    return GoBoardUtil::EndOfGame(m_board);
}

/** Find the game info node with a game info property that determines
    this property for the current node.
    Returns an empty string, if no such node exists. */
std::string GoGame::GetGameInfoStringProp(SgPropID id) const
{
    const SgNode* node = m_current->TopProp(id);
    if (node->HasProp(id))
    {
        const SgPropText* prop = dynamic_cast<SgPropText*>(node->Get(id));
        return prop->Value();
    }
    else
        return "";
}

std::string GoGame::GetGameName() const
{
    return GetGameInfoStringProp(SG_PROP_GAME_NAME);
}

std::string GoGame::GetPlayerName(SgBlackWhite player) const
{
    SgPropID id = SgProp::PlayerProp(SG_PROP_PLAYER_BLACK, player);
    return GetGameInfoStringProp(id);
}

std::string GoGame::GetResult() const
{
    return GetGameInfoStringProp(SG_PROP_RESULT);
}

void GoGame::GoInDirection(SgNode::Direction dir)
{
    SgNode* node = m_current->NodeInDirection(dir);
    if (node != m_current)
        GoToNode(node);
}

void GoGame::GoToNode(SgNode* dest)
{
    m_updater.Update(dest, m_board);
    SgNodeUtil::UpdateTime(Time(), dest);
    m_current = dest;
    if (GoBoardUtil::RemainingChineseHandicap(m_board))
        m_board.SetToPlay(SG_BLACK);
    m_time.EnterNode(*m_current, m_board.ToPlay());
}

void GoGame::Init(int size, const GoRules& rules)
{
    DeleteTreeAndInitState();
    m_board.Init(size, rules);
    SgNode* root = new SgNode();
    SgPropInt* boardSize = new SgPropInt(SG_PROP_SIZE, m_board.Size());
    root->Add(boardSize);
    GoKomi komi = rules.Komi();
    if (! komi.IsUnknown())
        root->SetRealProp(SG_PROP_KOMI, komi.ToFloat(), 1);
    InitHandicap(rules, root);
    GoToNode(root);
}

void GoGame::Init(SgNode* root)
{
    DeleteTreeAndInitState();
    if (root)
    {
        int size = GO_DEFAULT_SIZE;
        SgPropInt* boardSize =
            static_cast<SgPropInt*>(root->Get(SG_PROP_SIZE));
        if (boardSize)
        {
            size = boardSize->Value();
            ForceInRange(SG_MIN_SIZE, &size, SG_MAX_SIZE);
        }
        const GoRules& rules = m_board.Rules();
        m_board.Init(size, GoRules(rules.Handicap(), rules.Komi()));
    }
    else
    {
        // Create a new game tree, use current board.
        root = new SgNode();

        // Add root property: board size.
        SgPropInt* boardSize = new SgPropInt(SG_PROP_SIZE, m_board.Size());
        root->Add(boardSize);
    }

    // Add root property: Go game identifier.
    const int GAME_ID = 1;
    SgPropInt* gameId = new SgPropInt(SG_PROP_GAME, GAME_ID);
    root->Add(gameId);

    // Go to the root node.
    GoToNode(root);
}

void GoGame::PlaceHandicap(const SgVector<SgPoint>& stones)
{
    SG_ASSERT(m_board.TotalNumStones(SG_BLACK)
              + m_board.TotalNumStones(SG_WHITE) > 0);
    SgNode* node = m_current;
    if (node->HasSon())
        node = node->NewRightMostSon();
    SgPropAddStone* addBlack = new SgPropAddStone(SG_PROP_ADD_BLACK);
    for (SgVectorIterator<SgPoint> it(stones); it; ++it)
        addBlack->PushBack(*it);
    node->Add(addBlack);
    SgPropInt* handicap = new SgPropInt(SG_PROP_HANDICAP, stones.Length());
    node->Add(handicap);
    node->Add(new SgPropPlayer(SG_PROP_PLAYER, SG_WHITE));
    m_board.Rules().SetHandicap(stones.Length());
    GoToNode(node);
}

void GoGame::InitHandicap(const GoRules& rules, SgNode* root)
{
    // TODO: Use PlaceHandicap() in implementation of InitHandicap() to
    // avoid redundancy

    // Add handicap properties.
    if (2 <= rules.Handicap())
    {
        SgPropInt* handicap =
            new SgPropInt(SG_PROP_HANDICAP, rules.Handicap());
        root->Add(handicap);
        if (rules.JapaneseHandicap())
        {
            if (9 <= m_board.Size())
            {
                int h = rules.Handicap();
                int half = (m_board.Size()+1) / 2;
                SgVector<SgPoint> stones;
                if ((4 < h) && (h % 2 != 0))
                {
                    stones.PushBack(SgPointUtil::Pt(half, half));
                    --h;
                }
                if (13 <= m_board.Size())
                {
                    AddHandicap(m_board.Size(), 4, 4, &h, &stones);
                    if (0 < h)
                        AddHandicap(m_board.Size(), half, 4, &h, &stones);
                    if (0 < h)
                        AddHandicap(m_board.Size(), 3, 3, &h, &stones);
                    if (0 < h)
                        AddHandicap(m_board.Size(), 7, 7, &h, &stones);
                    if (0 < h)
                        AddHandicap(m_board.Size(), half, 3, &h, &stones);
                    if (0 < h)
                        AddHandicap(m_board.Size(), half - (half - 4) / 2,
                                    4, &h, &stones);
                    if (0 < h)
                        AddHandicap(m_board.Size(), half + (half - 4) / 2,
                                    4, &h, &stones);
                }
                else
                {
                    AddHandicap(m_board.Size(), 3, 3, &h, &stones);
                    if (0 < h)
                        AddHandicap(m_board.Size(), half, 3, &h, &stones);
                    if (0 < h)
                        AddHandicap(m_board.Size(), 4, 4, &h, &stones);
                }
                SgPropAddStone* addBlack =
                    new SgPropAddStone(SG_PROP_ADD_BLACK, stones);
                root->Add(addBlack);

                // White to play.
                SgPropPlayer* player =
                    new SgPropPlayer(SG_PROP_PLAYER, SG_WHITE);
                root->Add(player);
            }
        }
        else
        {
            // Chinese handicap.
            SgPropInt* chinese =
                new SgPropInt(SG_PROP_CHINESE, rules.Handicap());
            root->Add(chinese);
        }
    }
}

void GoGame::SetKomiGlobal(GoKomi komi)
{
    SgNode& root = *(m_current->Root());
    SgNodeUtil::RemovePropInSubtree(root, SG_PROP_KOMI);
    if (! komi.IsUnknown())
        root.SetRealProp(SG_PROP_KOMI, komi.ToFloat(), 1);
    m_board.Rules().SetKomi(komi);
}

void GoGame::SetRulesGlobal(const GoRules& rules)
{
    m_board.Rules() = rules;
    // TODO: Create description of rules and store it in RU property of root
    SetKomiGlobal(rules.Komi());
}

void GoGame::SetTimeSettingsGlobal(const GoTimeSettings& timeSettings,
                                   double overhead)
{
    m_timeSettings = timeSettings;
    SgNode& root = *(m_current->Root());
    SgNodeUtil::RemovePropInSubtree(root, SG_PROP_TIME);
    SgNodeUtil::RemovePropInSubtree(root, SG_PROP_OT_NU_MOVES);
    SgNodeUtil::RemovePropInSubtree(root, SG_PROP_OT_PERIOD);
    if (timeSettings.IsUnknown())
    {
        // TODO: What to do with m_time? What to do with time left properties
        // in tree nodes?
        return;
    }
    double mainTime = timeSettings.MainTime();
    root.Add(new SgPropTime(SG_PROP_TIME, mainTime));
    double overtime = timeSettings.Overtime();
    if (overtime > 0)
    {
        root.Add(new SgPropTime(SG_PROP_OT_PERIOD, overtime));
        root.SetIntProp(SG_PROP_OT_NU_MOVES, timeSettings.OvertimeMoves());
    }
    // TODO: What if the current node is not the root? What if nodes on the
    // path from the root to the current node contain time left properties?
    // Should we delete all time left properties or keep and still respect
    // them for setting the time left in the current position?
    m_time.SetOTPeriod(overtime);
    m_time.SetOTNumMoves(timeSettings.OvertimeMoves());
    m_time.SetOverhead(overhead);
    m_time.SetClock(*m_current, SG_BLACK, mainTime);
    m_time.SetClock(*m_current, SG_WHITE, mainTime);
    m_time.TurnClockOn(true);
}

void GoGame::SetToPlay(SgBlackWhite toPlay)
{
    if (toPlay == m_board.ToPlay())
        return;
    m_board.SetToPlay(toPlay);
    m_current->Add(new SgPropPlayer(SG_PROP_PLAYER, toPlay));
    m_time.EnterNode(*m_current, toPlay);
}

void GoGame::UpdateDate(const std::string& date)
{
    UpdateGameInfoStringProp(SG_PROP_DATE, date);
}

void GoGame::UpdateGameInfoStringProp(SgPropID id, const std::string& value)
{
    SgNode* node = m_current->TopProp(id);
    node->SetStringProp(id, value);
}

void GoGame::UpdateGameName(const std::string& name)
{
    UpdateGameInfoStringProp(SG_PROP_GAME_NAME, name);
}

void GoGame::UpdatePlayerName(SgBlackWhite color, const std::string& name)
{
    SgPropID id = SgProp::PlayerProp(SG_PROP_PLAYER_BLACK, color);
    UpdateGameInfoStringProp(id, name);
}

void GoGame::UpdateResult(const std::string& result)
{
    UpdateGameInfoStringProp(SG_PROP_RESULT, result);
}

//----------------------------------------------------------------------------

bool GoGameUtil::GotoBeforeMove(GoGame* game, int moveNumber)
{
    SG_ASSERT(game->CurrentNode() == &game->Root());
    SG_ASSERT(moveNumber == -1 || moveNumber > 0);
    if (moveNumber > 0)
    {
        while (game->CanGoInDirection(SgNode::NEXT)
               && ! game->CurrentNode()->HasProp(SG_PROP_MOVE)
               && ! game->CurrentNode()->LeftMostSon()->HasProp(SG_PROP_MOVE))
            game->GoInDirection(SgNode::NEXT);
        while (game->CurrentMoveNumber() < moveNumber - 1
               && game->CanGoInDirection(SgNode::NEXT))
            game->GoInDirection(SgNode::NEXT);
        if (game->CurrentMoveNumber() != moveNumber - 1)
            return false;
    }
    else
    {
        while (game->CanGoInDirection(SgNode::NEXT))
            game->GoInDirection(SgNode::NEXT);
    }
    return true;
}

//----------------------------------------------------------------------------
