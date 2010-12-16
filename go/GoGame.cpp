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

GoGameRecord::GoGameRecord(int boardSize)
    : m_board(boardSize),
      m_current(0),
      m_time(),
      m_oldCommentNode(0),
      m_numMovesToInsert(0)
{
    // Make sure GoInit was called to avoid silent failure of ExecuteMove
    // because of unregistered move property
    GoInitCheck();
    InitFromRoot(0);
}

GoGameRecord::~GoGameRecord()
{
    m_current->Root()->DeleteTree();
}


SgNode* GoGameRecord::AddMove(SgMove move, SgBlackWhite player,
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

SgNode* GoGameRecord::AddResignNode(SgBlackWhite player)
{
    SgNode* node = m_current->NewRightMostSon();
    ostringstream comment;
    comment << (player == SG_BLACK ? "Black" : "White") << " resigned";
    node->AddComment(comment.str());
    return node;
}

bool GoGameRecord::CanGoInDirection(SgNode::Direction dir) const
{
    SgNode* node = m_current->NodeInDirection(dir);
    return node && node != m_current;
}

SgMove GoGameRecord::CurrentMove() const
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

int GoGameRecord::CurrentMoveNumber() const
{
    // TODO: once the transition of GoBoard to only support setup stones
    // in the initial position is finished, it will be more efficient to
    // call m_board.MoveNumber() instead of SgNodeUtil::GetMoveNumber()
    return SgNodeUtil::GetMoveNumber(m_current);
}

void GoGameRecord::DeleteTreeAndInitState()
{
    if (m_current)
        m_current->Root()->DeleteTree();
    m_current = 0;
    m_oldCommentNode = 0;
}

bool GoGameRecord::EndOfGame() const
{
    return GoBoardUtil::EndOfGame(m_board);
}

/** Find the game info node with a game info property that determines
    this property for the current node.
    Returns an empty string, if no such node exists. */
std::string GoGameRecord::GetGameInfoStringProp(SgPropID id) const
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

std::string GoGameRecord::GetGameName() const
{
    return GetGameInfoStringProp(SG_PROP_GAME_NAME);
}

std::string GoGameRecord::GetPlayerName(SgBlackWhite player) const
{
    SgPropID id = SgProp::PlayerProp(SG_PROP_PLAYER_BLACK, player);
    return GetGameInfoStringProp(id);
}

std::string GoGameRecord::GetResult() const
{
    return GetGameInfoStringProp(SG_PROP_RESULT);
}

void GoGameRecord::GoInDirection(SgNode::Direction dir)
{
    SgNode* node = m_current->NodeInDirection(dir);
    if (node != m_current)
        GoToNode(node);
}

void GoGameRecord::GoToNode(SgNode* dest)
{
    m_updater.Update(dest, m_board);
    SgNodeUtil::UpdateTime(Time(), dest);
    m_current = dest;
    if (GoBoardUtil::RemainingChineseHandicap(m_board))
        m_board.SetToPlay(SG_BLACK);
    m_time.EnterNode(*m_current, m_board.ToPlay());
    OnGoToNode(dest);
}

void GoGameRecord::Init(int size, const GoRules& rules)
// Delete the old game record and start with a fresh one. Init the board
// to the given size and handicap, and create a root node to start with.
{
    DeleteTreeAndInitState();
    m_board.Init(size, rules);
    // Create a new game tree, use current board.
    SgNode* root = new SgNode();
    // Add root property: board size.
    SgPropInt* boardSize = new SgPropInt(SG_PROP_SIZE, m_board.Size());
    root->Add(boardSize);
    InitHandicap(rules, root);
    GoToNode(root);
}

void GoGameRecord::InitFromRoot(SgNode* root)
{
    DeleteTreeAndInitState();
    if (root)
    {
        // Get board properties from root node.
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

void GoGameRecord::OnGoToNode(SgNode* dest)
{
    SG_UNUSED(dest);
}

void GoGameRecord::InitHandicap(const GoRules& rules, SgNode* root)
{
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

void GoGameRecord::SetToPlay(SgBlackWhite player)
{
    if (player != m_board.ToPlay())
    {
        m_board.SetToPlay(player);
        if (m_current)
        {
            // Also record this change of player in the move tree.
            m_current->Add(new SgPropPlayer(SG_PROP_PLAYER, player));

            // Update the clock.
            m_time.EnterNode(*m_current, player);
        }
    }
}

void GoGameRecord::UpdateGameInfoStringProp(SgPropID id,
                                            const std::string& value)
{
    SgNode* node = m_current->TopProp(id);
    node->SetStringProp(id, value);
}

void GoGameRecord::UpdateGameName(const std::string& name)
{
    UpdateGameInfoStringProp(SG_PROP_GAME_NAME, name);
}

void GoGameRecord::UpdatePlayerName(SgBlackWhite color,
                                    const std::string& name)
{
    SgPropID id = SgProp::PlayerProp(SG_PROP_PLAYER_BLACK, color);
    UpdateGameInfoStringProp(id, name);
}

void GoGameRecord::UpdateResult(const std::string& result)
{
    UpdateGameInfoStringProp(SG_PROP_RESULT, result);
}

//----------------------------------------------------------------------------

bool GoGameUtil::GotoBeforeMove(GoGameRecord* game, int moveNumber)
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

GoGame::GoGame(int boardSize)
    : GoGameRecord(boardSize),
      m_player(0)
{
}

GoGame::~GoGame()
{
    DeletePlayer(SG_BLACK);
    DeletePlayer(SG_WHITE);
}

bool GoGame::CanPlayHumanMove(SgMove move, SgBlackWhite player) const
{
    return    (  m_player[player] == 0
              || ! ClockIsRunning()
              )
           && Board().IsLegal(move, player);
}

void GoGame::DeletePlayer(SgBlackWhite color)
{
    SG_ASSERT_BW(color);
    if (m_player[SG_BLACK] == m_player[SG_WHITE])
    {
        delete m_player[SG_BLACK];
        m_player[SG_BLACK] = 0;
        m_player[SG_WHITE] = 0;
        return;
    }
    delete m_player[color];
    m_player[color] = 0;
}

void GoGame::Init(int size, const GoRules& rules)
{
    GoGameRecord::Init(size, rules);
    UpdatePlayers();
}

void GoGame::Init(SgNode* root, bool fDeletePlayers)
{
    if (fDeletePlayers)
    {
        DeletePlayer(SG_BLACK);
        DeletePlayer(SG_WHITE);
    }
    GoGameRecord::InitFromRoot(root);
    UpdatePlayers();
}

void GoGame::Init(int size, const GoRules& rules, bool fDeletePlayers)
{
    if (fDeletePlayers)
    {
        DeletePlayer(SG_BLACK);
        DeletePlayer(SG_WHITE);
    }
    GoGameRecord::Init(size, rules);
    UpdatePlayers();
}

void GoGame::OnGoToNode(SgNode* dest)
{
    SG_UNUSED(dest);
    UpdatePlayers();
}

void GoGame::PlayComputerMove(const GoPlayerMove* playerMove)
{
    SgBlackWhite toPlay = playerMove->Color();
    SgPoint move = playerMove->Point();
    SgNode* node = AddMove(move, toPlay, 0);
    GoToNode(node);
}

GoPlayerMove GoGame::PlayOneMove(SgBlackWhite color)
{
    TurnClockOn(false);
    SgTimeRecord time(/*fOneMoveOnly*/true, 20);
    GoPlayer* player = m_player[color];
    UpdatePlayer(color);
    SgBlackWhite toPlay = Board().ToPlay();
    SgMove move = player->GenMove(time, toPlay);
    return GoPlayerMove(toPlay, move);
}

bool GoGame::PlayHumanMove(SgMove move, SgBlackWhite player)
{
    if (CanPlayHumanMove(move, player))
    {
        // Add the move to the tree.
        SgNode* node = AddMove(move, player);
        GoToNode(node);
        return true;
    }
    // Illegal move, or not human player's turn to play.
    return false;
}

void GoGame::SetPlayer(SgBlackWhite color, GoPlayer* player)
{
    SG_ASSERT_BW(color);
    if (! (m_player[SG_BLACK] == m_player[SG_WHITE]))
        DeletePlayer(color);
    m_player[color] = player;
    UpdatePlayers();
}

void GoGame::TurnClockOn(bool turnOn)
{
    Time().TurnClockOn(turnOn);
}

void GoGame::UpdatePlayer(SgBlackWhite color)
{
    GoPlayer* player = m_player[color];
    if (player == 0)
        return;
    player->UpdateSubscriber();
    player->SetCurrentNode(CurrentNode());
}

void GoGame::UpdatePlayers()
{
    UpdatePlayer(SG_BLACK);
    if (m_player[SG_BLACK] != m_player[SG_WHITE])
        UpdatePlayer(SG_WHITE);
}

//----------------------------------------------------------------------------

