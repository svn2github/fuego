//----------------------------------------------------------------------------
/** @file SgProp.cpp
    See SgProp.h.

    Implementation details:

    A property list is simply kept as a list.
    Any special characteristics of the
    property list are enforced when adding new items to the list.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgProp.h"

#include <iomanip>
#include <sstream>
#include "SgList.h"
#include "SgRect.h"
#include "SgUtil.h"

using namespace std;
using SgPointUtil::InBoardRange;
using SgPropUtil::PointToSgfString;
using SgPropUtil::SgfStringToPoint;
using SgPropUtil::EscapeSpecialCharacters;
using SgUtil::InRange;

//----------------------------------------------------------------------------

SgPropPointFmt SgPropUtil::GetPointFmt(int gameNumber)
{
    switch (gameNumber)
    {
    case 2: // Othello
    case 11: // Hex
        return SG_PROPPOINTFMT_HEX;
    default:
        return SG_PROPPOINTFMT_GO;
    }
}

string SgPropUtil::PointToSgfString(SgMove p, int boardSize,
                                    SgPropPointFmt fmt, int fileFormat)
{
    SG_ASSERT(boardSize >= SG_MIN_SIZE && boardSize <= SG_MAX_SIZE);
    SG_ASSERT(p != SG_NULLMOVE);
    ostringstream out;
    switch (fmt)
    {
    case SG_PROPPOINTFMT_GO:
        {
            if (p == SG_PASS)
            {
                if (fileFormat < 4)
                    out << "tt";
                // Pass is empty string in FF[4]
            }
            else
            {
                int col = SgPointUtil::Col(p);
                int row = boardSize - SgPointUtil::Row(p) + 1;
                SG_ASSERT(row > 0);
                out << static_cast<char>('a' + col - 1)
                    << static_cast<char>('a' + row - 1);
            }
        }
        break;
    case SG_PROPPOINTFMT_HEX:
        {
            SG_ASSERT(p != SG_PASS);
            int col = SgPointUtil::Col(p);
                int row = boardSize - SgPointUtil::Row(p) + 1;
            out << static_cast<char>('a' + col - 1) << row;
        }
        break;
    default:
        SG_ASSERT(false);
    }
    return out.str();
}

string SgPropUtil::EscapeSpecialCharacters(const string& s, bool escapeColon)
{
    ostringstream buffer;
    for (size_t i = 0; i < s.size(); ++i)
    {
        char c = s[i];
        if (c == ']' || c =='\\' || (c == ':' && escapeColon))
            buffer << '\\' << c;
        else if (c == '\r' || c == '\t')
            buffer << ' ';
        else
            buffer << c;
    }
    return buffer.str();
}

SgMove SgPropUtil::SgfStringToPoint(const string& s, int boardSize,
                                    SgPropPointFmt fmt)
{
    SG_ASSERT(boardSize >= SG_MIN_SIZE && boardSize <= SG_MAX_SIZE);
    SgPoint p = SG_NULLMOVE;
    switch (fmt)
    {
    case SG_PROPPOINTFMT_GO:
        {
            if (s.size() == 2
                && 'a' <= s[0] && s[0] <= 's'
                && 'a' <= s[1] && s[1] <= 's')
            {
                int col = s[0] - 'a' + 1;
                int row = s[1] - 'a' + 1;
                p = SgPointUtil::Pt(col, boardSize - row + 1);
            }
            else if (s.empty() // new FF[4] definition
                     || (s.size() == 2 && s[0] == 't' && s[1] == 't'))
                p = SG_PASS;
        }
        break;
    case SG_PROPPOINTFMT_HEX:
        {
            if (s.size() >= 2 && s.size() <= 3)
            {
                int col = s[0] - 'a' + 1;
                int row = s[1] - '1' + 1;
                if (s.size() == 3)
                    row = row * 10 + (s[2] - '1' + 1);
                if (InRange(col, 1, boardSize) && InRange(row, 1, boardSize))
                    p = SgPointUtil::Pt(col, boardSize - row + 1);
            }
        }
        break;
    default:
        SG_ASSERT(false);
    }
    return p;
}

//----------------------------------------------------------------------------

SgPropList::SgPropList()
    : m_list()
{ }

SgPropList::~SgPropList()
{
    Clear();
}

void SgPropList::Clear()
{
    for (SgListIteratorOf<SgProp> iter(m_list); iter; ++iter)
        delete *iter;
    m_list.Clear();
}

SgProp* SgPropList::Get(SgPropID id) const
{
    for (SgListIteratorOf<SgProp> iter(m_list); iter; ++iter)
    {
        SgProp* prop = *iter;
        if (prop->MatchesID(id))
            return prop;
    }
    return 0;
}

void SgPropList::Add(const SgProp* prop)
{
    SG_ASSERT(prop);

    // First remove any property of that type, then add new property.
    // Enforce constraint that there can be only one move annotation per node.
    // Can have multiple unknown properties per node.
    if (prop->ID() != SG_PROP_UNKNOWN)
    {
        if (prop->Flag(fMoveAnno))
            Remove(SG_PROP_MOVE_ANNO, prop);
        else if (prop->Flag(fPosAnno))
            Remove(SG_PROP_POS_ANNO, prop);
        else if (prop->Flag(fMoveProp))
            Remove(SG_PROP_MOVE, prop);
        else
            Remove(prop->ID(), prop);
    }
    m_list.Include(prop);
}

void SgPropList::MoveToFront(SgPropID id)
{
    SgProp* prop = Get(id);
    if (prop && m_list.Exclude(prop))
        m_list.Push(prop);
}

bool SgPropList::Remove(const SgProp* prop)
{
    if (prop)
        delete prop;
    return m_list.Exclude(const_cast<SgProp*>(prop));
}

void SgPropList::Remove(SgPropID id, const SgProp* protectProp)
{
    SgListOf<SgProp> toBeDeleted;
    for (SgListIteratorOf<SgProp> iter(m_list); iter; ++iter)
    {
        SgProp* prop = *iter;
        if (prop != protectProp && prop->MatchesID(id))
        {
            // Can't exclude while iterating over same list.
            toBeDeleted.Append(prop);
            delete prop;
        }
    }
    m_list.Exclude(toBeDeleted);
}

bool SgPropList::AppendMoveAnnotation(string* s) const
{
    SgProp* moveAnnoProp = Get(SG_PROP_MOVE_ANNO);
    if (! moveAnnoProp)
        return false;
    SgPropInt* intProp = dynamic_cast<SgPropInt*>(moveAnnoProp);
    int value = intProp ? intProp->Value() : 1;
    SgPropID id = moveAnnoProp->ID();
    if (id == SG_PROP_GOOD_MOVE)
        *s += (value == 2) ? "!!" : "!";
    else if (id == SG_PROP_BAD_MOVE)
        *s += (value == 2) ? "??" : "?";
    else if (id == SG_PROP_INTERESTING)
        *s += "!?";
    else if (id == SG_PROP_DOUBTFUL)
        *s += "?!";
    return true;
}

SgProp* SgPropList::GetPropContainingText(const string& findText) const
{
    for (SgListIteratorOf<SgProp> iter(m_list); iter; ++iter)
    {
        SgProp* prop = *iter;
        if (prop->ContainsText(findText))
            return prop;
    }
    return 0;
}

//----------------------------------------------------------------------------

//--- general
SgPropID SG_PROP_NONE = 0;
SgPropID SG_PROP_UNKNOWN = 0;

//--- moves
SgPropID SG_PROP_MOVE = 0;
SgPropID SG_PROP_MOVE_BLACK = 0;
SgPropID SG_PROP_MOVE_WHITE = 0;

//--- board edits
SgPropID SG_PROP_ADD_BLACK = 0;
SgPropID SG_PROP_ADD_WHITE = 0;
SgPropID SG_PROP_ADD_EMPTY = 0;
SgPropID SG_PROP_PLAYER = 0;

//--- value and territory
SgPropID SG_PROP_VALUE = 0;
SgPropID SG_PROP_TERR_BLACK = 0;
SgPropID SG_PROP_TERR_WHITE = 0;

//--- marks drawn on the board
SgPropID SG_PROP_MARKS = 0;
SgPropID SG_PROP_SELECT = 0;
SgPropID SG_PROP_MARKED = 0;
SgPropID SG_PROP_TRIANGLE = 0;
SgPropID SG_PROP_SQUARE = 0;
SgPropID SG_PROP_DIAMOND = 0;
SgPropID SG_PROP_CIRCLE = 0;
SgPropID SG_PROP_DIMMED = 0;
SgPropID SG_PROP_LABEL = 0;

//--- time control
SgPropID SG_PROP_TIMES = 0;
SgPropID SG_PROP_TIME_BLACK = 0;
SgPropID SG_PROP_TIME_WHITE = 0;
SgPropID SG_PROP_OT_NU_MOVES = 0;
SgPropID SG_PROP_OT_PERIOD = 0;
SgPropID SG_PROP_OT_BLACK = 0;
SgPropID SG_PROP_OT_WHITE = 0;
SgPropID SG_PROP_LOSE_TIME = 0;
SgPropID SG_PROP_OVERHEAD = 0;

//--- statistics
SgPropID SG_PROP_COUNT = 0;
SgPropID SG_PROP_TIME_USED = 0;
SgPropID SG_PROP_NUM_NODES = 0;
SgPropID SG_PROP_NUM_LEAFS = 0;
SgPropID SG_PROP_MAX_DEPTH = 0;
SgPropID SG_PROP_DEPTH = 0;
SgPropID SG_PROP_PART_DEPTH = 0;
SgPropID SG_PROP_EVAL = 0;
SgPropID SG_PROP_EXPECTED = 0;

//--- root props
SgPropID SG_PROP_FORMAT = 0;
SgPropID SG_PROP_SIZE = 0;
SgPropID SG_PROP_GAME = 0;
SgPropID SG_PROP_SPEC_BLACK = 0;
SgPropID SG_PROP_SPEC_WHITE = 0;
SgPropID SG_PROP_CHINESE = 0;
SgPropID SG_PROP_APPLIC = 0;

//--- annotations
SgPropID SG_PROP_ANNOTATE = 0;
SgPropID SG_PROP_COMMENT = 0;
SgPropID SG_PROP_NAME = 0;
SgPropID SG_PROP_CHECK = 0;
SgPropID SG_PROP_SIGMA = 0;
SgPropID SG_PROP_HOTSPOT = 0;
SgPropID SG_PROP_FIGURE = 0;

//--- position annotations
SgPropID SG_PROP_POS_ANNO = 0;
SgPropID SG_PROP_GOOD_BLACK = 0;
SgPropID SG_PROP_GOOD_WHITE = 0;
SgPropID SG_PROP_EVEN_POS = 0;
SgPropID SG_PROP_UNCLEAR = 0;

//--- move annotations
SgPropID SG_PROP_MOVE_ANNO = 0;
SgPropID SG_PROP_GOOD_MOVE = 0;
SgPropID SG_PROP_BAD_MOVE = 0;
SgPropID SG_PROP_INTERESTING = 0;
SgPropID SG_PROP_DOUBTFUL = 0;

//--- game info
SgPropID SG_PROP_INFO = 0;
SgPropID SG_PROP_GAME_NAME = 0;
SgPropID SG_PROP_GAME_COMMENT = 0;
SgPropID SG_PROP_EVENT = 0;
SgPropID SG_PROP_ROUND = 0;
SgPropID SG_PROP_DATE = 0;
SgPropID SG_PROP_PLACE = 0;
SgPropID SG_PROP_PLAYER_BLACK = 0;
SgPropID SG_PROP_PLAYER_WHITE = 0;
SgPropID SG_PROP_RESULT = 0;
SgPropID SG_PROP_USER = 0;
SgPropID SG_PROP_TIME = 0;
SgPropID SG_PROP_SOURCE = 0;
SgPropID SG_PROP_COPYRIGHT = 0;
SgPropID SG_PROP_ANALYSIS = 0;
SgPropID SG_PROP_RANK_BLACK = 0;
SgPropID SG_PROP_RANK_WHITE = 0;
SgPropID SG_PROP_TEAM_BLACK = 0;
SgPropID SG_PROP_TEAM_WHITE = 0;
SgPropID SG_PROP_OPENING = 0;
SgPropID SG_PROP_RULES = 0;
SgPropID SG_PROP_HANDICAP = 0;
SgPropID SG_PROP_KOMI = 0;

//--- abstract properties
SgPropID SG_PROP_FIND_MOVE = 0;
SgPropID SG_PROP_FIND_TEXT = 0;
SgPropID SG_PROP_BRANCH = 0;
SgPropID SG_PROP_TERMINAL = 0;

//--- Smart Go specific properties
SgPropID SG_PROP_MOTIVE = 0;
SgPropID SG_PROP_SEQUENCE = 0;
SgPropID SG_PROP_NOT_EMPTY = 0;
SgPropID SG_PROP_NOT_BLACK = 0;
SgPropID SG_PROP_NOT_WHITE = 0;

//----------------------------------------------------------------------------

bool SgProp::s_initialized = false;

int SgProp::s_numPropClasses = 0;

SgPropFlags SgProp::s_flags[MAX_PROP_CLASS];

string SgProp::s_label[MAX_PROP_CLASS];

SgProp* SgProp::s_prop[MAX_PROP_CLASS];

SgProp::~SgProp()
{
}

void SgProp::ChangeToOpponent()
{
    m_id = OpponentProp(m_id);
}

bool SgProp::ContainsText(const std::string& findText)
{
    SG_UNUSED(findText);
    return false;
}

SgPropFlags SgProp::Flags() const
{
    return s_flags[m_id];
}

bool SgProp::Initialized()
{
    return s_initialized;
}

string SgProp::Label() const
{
    return s_label[m_id];
}

SgPropID SgProp::Register(SgProp* prop, const char* label, SgPropFlags flags)
{
    ++s_numPropClasses;
    SG_ASSERT(s_numPropClasses < MAX_PROP_CLASS);
    if (s_numPropClasses < MAX_PROP_CLASS)
    {
        s_flags[s_numPropClasses] = flags;
        s_label[s_numPropClasses] = label;
        s_prop[s_numPropClasses] = prop;
        if (prop)
        {
            SG_ASSERT(prop->m_id == 0); // can't know the ID yet
        }
        // Black and white properties must be created in pairs, black first.
        if (flags & fWhiteProp)
            SG_ASSERT(s_flags[s_numPropClasses-1] & fBlackProp);

        return s_numPropClasses;
    }
    else
        return 0;
}

SgProp* SgProp::CreateProperty(SgPropID id)
{
    SG_ASSERT(id <= s_numPropClasses);
    SG_ASSERT(s_prop[id]);

    // Create a property of the right class, and set its ID (because
    // prototype that's duplicated contains a zero ID).
    SgProp* prop = s_prop[id]->Duplicate();
    prop->m_id = id;
    return prop;
}

SgPropID SgProp::GetIDOfLabel(const string& label)
{
    for (int i = 1; i <= s_numPropClasses; ++i)
        if (s_label[i] == label)
            return i;
    return SG_PROP_NONE;
}

SgPropID SgProp::OpponentProp(SgPropID id)
{
    // Relies on the fact that these properties are always created in pairs,
    // with black created before white.
    // AR: ---> Flags cannot really be overridden
    SgPropFlags flags = s_flags[id];

    if (flags & fBlackProp)
    {
        ++id;
        SG_ASSERT(s_flags[id] & fWhiteProp);
    }
    else if (flags & fWhiteProp)
    {
        --id;
        SG_ASSERT(s_flags[id] & fBlackProp);
    }
    return id;
}

SgPropID SgProp::PlayerProp(SgPropID id, SgBlackWhite player)
{
    // AR: ---> Flags cannot really be overridden
    SgPropFlags flags = s_flags[id];
    SG_ASSERT(flags & (fBlackProp | fWhiteProp));
    SG_ASSERT(fBlackProp == SG_BLACK && fWhiteProp == SG_WHITE);
    if (flags & player)
        return id;
    else
        return OpponentProp(id);
}

int SgProp::Player() const
{
    SG_ASSERT(Flag(fBlackProp | fWhiteProp));
    return Flags() & (fBlackProp | fWhiteProp);
}

bool SgProp::IsPlayer(SgBlackWhite player) const
{
    SG_ASSERT_BW(player);
    SG_ASSERT(SG_BLACK == fBlackProp); // warn if implementation changes
    SG_ASSERT(SG_WHITE == fWhiteProp); // warn if implementation changes
    SG_ASSERT(Flag(fBlackProp | fWhiteProp));
    SG_ASSERT((Flags() & fBlackProp) != (Flags() & fWhiteProp));
    return ((Flags() & player) != 0);
}

bool SgProp::MatchesID(SgPropID id) const
{
    // Matches if ID matches exactly.
    if (id == ID())
        return true;

    // Matches if looking for abstract property and matches that category.
    if (s_flags[id] & fAbstract)
    {
        SgPropFlags fCategories = s_flags[id] & (~fAbstract);
        if ((Flags() & fCategories) == fCategories)
            return true;
    }

    return false;
}

SgPropID SgProp::ConvertFindTextToPropID(const string& findText)
{
    size_t length = findText.size();
    if (   (3 <= length && findText[1] == ' ' && findText[2] == '<')
        || (2 <= length && findText[1] == '<')
        ||  1 == length)
    {
        switch (findText[0])
        {
        case 'A': return SG_PROP_ANNOTATE;
        case 'B': return SG_PROP_MOVE_BLACK;
        case 'C': return SG_PROP_COMMENT;
        case 'H': return SG_PROP_HOTSPOT;
        case 'I': return SG_PROP_INFO;
        case 'K': return SG_PROP_CHECK;
        case 'M': return SG_PROP_MARKS;
        case 'N': return SG_PROP_NAME;
        case 'S': return SG_PROP_SIGMA;
        case 'T': return SG_PROP_TRIANGLE;
        case 'W': return SG_PROP_MOVE_WHITE;
        case '!': return SG_PROP_GOOD_MOVE;
        case '?': return SG_PROP_BAD_MOVE;
        case '.': return SG_PROP_TERMINAL;
        case ':': return SG_PROP_BRANCH;
        default: break;
        }
    }
    return SG_PROP_NONE;
}

void SgProp::Init()
{
    // Attributes of SG_PROP_NONE.
    s_flags[0] = 0;
    s_label[0] = "";
    s_prop[0] = 0;

    // Create prototype properties, one for each property class.
    SgProp* unknownProp = new SgPropUnknown(0);
    SgProp* simpleProp = new SgPropSimple(0);
    SgProp* intProp = new SgPropInt(0);
    SgProp* realProp = new SgPropReal(0);
    SgProp* multipleProp = new SgPropMultiple(0);
    SgProp* valueProp = new SgPropValue(0);
    SgProp* timeProp = new SgPropTime(0);
    SgProp* mSecProp = new SgPropMSec(0);
    SgProp* moveProp = new SgPropMove(0);
    SgProp* listProp = new SgPropPointList(0);
    SgProp* textProp = new SgPropText(0);
    SgProp* textListProp = new SgPropTextList(0);
    SgProp* playerProp = new SgPropPlayer(0);
    SgProp* addStoneProp = new SgPropAddStone(0);

    // Register abstract property classes so they get cleaned up on fini.
    SG_PROP_NONE = 0;
    SG_PROP_UNKNOWN = Register(unknownProp, "");
    Register(simpleProp, "");
    Register(intProp, "");
    Register(realProp, "");
    Register(multipleProp, "");
    Register(valueProp, "");
    Register(timeProp, "");
    Register(mSecProp, "");
    Register(moveProp, "");
    Register(listProp, "");
    Register(textProp, "");
    Register(textListProp, "");
    Register(playerProp, "");
    Register(addStoneProp, "");

    // Create the standard properties.

    //--- moves and board edits
    SG_PROP_MOVE = Register(0, "", fMoveProp+fAbstract);
    SG_PROP_PLAYER = Register(playerProp, "PL");
    SG_PROP_ADD_BLACK = Register(addStoneProp, "AB", fBlackProp + fNewLine);
    SG_PROP_ADD_WHITE = Register(addStoneProp, "AW", fWhiteProp + fNewLine);
    SG_PROP_ADD_EMPTY = Register(addStoneProp, "AE", fNewLine);

    //--- value and territory
    SG_PROP_VALUE = Register(valueProp, "V" );
    SG_PROP_TERR_BLACK = Register(listProp, "TB", fBlackProp + fNewLine);
    SG_PROP_TERR_WHITE = Register(listProp, "TW", fWhiteProp + fNewLine);

    //--- marks drawn on the board
    SG_PROP_MARKS = Register(0, "", fMarkProp + fAbstract);
    SG_PROP_SELECT = Register(listProp, "SL", fMarkProp);
    SG_PROP_MARKED = Register(listProp, "MA", fMarkProp);
    SG_PROP_TRIANGLE = Register(listProp, "TR", fMarkProp);
    SG_PROP_SQUARE = Register(listProp, "SQ", fMarkProp);
    SG_PROP_DIAMOND = Register(listProp, "RG", fMarkProp);
    SG_PROP_CIRCLE = Register(listProp, "CR", fMarkProp);
    SG_PROP_DIMMED = Register(listProp, "DD", fMarkProp);
    SG_PROP_LABEL = Register(textListProp, "LB", fMarkProp);

    //--- time control
    SG_PROP_TIMES = Register(0, "", fTimeProp+fAbstract);
    SG_PROP_TIME_BLACK = Register(timeProp, "BL", fTimeProp + fBlackProp);
    SG_PROP_TIME_WHITE = Register(timeProp, "WL", fTimeProp + fWhiteProp);
    SG_PROP_OT_BLACK =
        Register(intProp, "OB", fTimeProp + fBlackProp + fNotClean);
    SG_PROP_OT_WHITE =
        Register(intProp, "OW", fTimeProp + fWhiteProp + fNotClean);
    SG_PROP_OT_NU_MOVES =
        Register(intProp, "OM", fTimeProp + fRootProp + fCustom + fNotClean);
    SG_PROP_OT_PERIOD =
        Register(timeProp, "OP", fTimeProp + fRootProp + fCustom + fNotClean);
    SG_PROP_OVERHEAD =
        Register(timeProp, "OV", fTimeProp + fRootProp + fCustom + fNotClean);
    SG_PROP_LOSE_TIME =
        Register(simpleProp, "LT",
                 fTimeProp + fRootProp + fCustom + fNotClean);

    //--- statistics
    // AR: is official property?
    SG_PROP_COUNT = Register(0, "CN", fStatProp + fAbstract);
    SG_PROP_TIME_USED =
        Register(mSecProp, "TU", fStatProp + fCustom + fNotClean);
    SG_PROP_NUM_NODES =
        Register(intProp, "NN", fStatProp + fCustom + fNotClean);
    SG_PROP_NUM_LEAFS =
        Register(intProp, "NL", fStatProp + fCustom + fNotClean);
    SG_PROP_MAX_DEPTH =
        Register(intProp, "MD", fStatProp + fCustom + fNotClean);
    SG_PROP_DEPTH = Register(intProp, "DE", fStatProp + fCustom + fNotClean);
    SG_PROP_PART_DEPTH =
        Register(intProp, "PD", fStatProp + fCustom + fNotClean);
    SG_PROP_EVAL =
        Register(valueProp, "EL", fStatProp + fCustom + fNotClean);
    SG_PROP_EXPECTED =
        Register(moveProp, "EX", fStatProp + fCustom + fNotClean);

    //--- root props
    SG_PROP_FORMAT = Register(intProp, "FF", fRootProp);
    SG_PROP_SIZE = Register(intProp, "SZ", fRootProp);
    SG_PROP_GAME = Register(intProp, "GM", fRootProp);
    // AR: not root props?
    SG_PROP_SPEC_BLACK = Register(intProp, "BS", fCustom);
    SG_PROP_SPEC_WHITE = Register(intProp, "WS", fCustom);
    SG_PROP_CHINESE = Register(intProp, "CI", fRootProp + fCustom);
    SG_PROP_APPLIC = Register(textProp, "AP", fRootProp + fNotFF3);

    //--- annotations
    SG_PROP_ANNOTATE = Register(0, "", fAnnoProp + fAbstract);
    SG_PROP_COMMENT = Register(textProp, "C", fAnnoProp + fNewLine);
    SG_PROP_NAME = Register(textProp, "N", fAnnoProp + fNewLine);
    SG_PROP_CHECK = Register(multipleProp, "CH", fAnnoProp + fCustom);
    SG_PROP_SIGMA = Register(multipleProp, "SI", fAnnoProp + fCustom);
    SG_PROP_HOTSPOT = Register(multipleProp, "HO", fAnnoProp + fCustom);
    SG_PROP_FIGURE = Register(simpleProp, "FG", fAnnoProp + fNewLine);

    //--- position annotations
    SG_PROP_POS_ANNO = Register(0, "", fAnnoProp + fPosAnno + fAbstract);
    SG_PROP_GOOD_BLACK =
        Register(multipleProp, "GB", fAnnoProp + fPosAnno + fBlackProp);
    SG_PROP_GOOD_WHITE =
        Register(multipleProp, "GW", fAnnoProp + fPosAnno + fWhiteProp);
    SG_PROP_EVEN_POS = Register(multipleProp, "DM", fAnnoProp + fPosAnno);
    SG_PROP_UNCLEAR = Register(multipleProp, "UC", fAnnoProp + fPosAnno);

    //--- move annotations
    SG_PROP_MOVE_ANNO = Register(0, "", fAnnoProp + fMoveAnno + fAbstract);
    SG_PROP_GOOD_MOVE = Register(multipleProp, "TE", fAnnoProp + fMoveAnno);
    SG_PROP_BAD_MOVE = Register(multipleProp, "BM", fAnnoProp + fMoveAnno);
    SG_PROP_INTERESTING = Register(simpleProp, "IT", fAnnoProp + fMoveAnno);
    SG_PROP_DOUBTFUL = Register(simpleProp, "DO", fAnnoProp + fMoveAnno);

    //--- game info
    SG_PROP_INFO = Register(0, "", fInfoProp + fAbstract);
    SG_PROP_GAME_NAME = Register(textProp, "GN", fInfoProp + fNewLine);
    SG_PROP_GAME_COMMENT = Register(textProp, "GC", fInfoProp + fNewLine);
    SG_PROP_EVENT = Register(textProp, "EV", fInfoProp + fNewLine);
    SG_PROP_ROUND = Register(textProp, "RO", fInfoProp);
    SG_PROP_DATE = Register(textProp, "DT", fInfoProp + fNewLine);
    SG_PROP_PLACE = Register(textProp, "PC", fInfoProp + fNewLine);
    SG_PROP_PLAYER_BLACK =
        Register(textProp, "PB", fInfoProp + fBlackProp + fNewLine);
    SG_PROP_PLAYER_WHITE =
        Register(textProp, "PW", fInfoProp + fWhiteProp + fNewLine);
    SG_PROP_RESULT = Register(textProp, "RE", fInfoProp + fNewLine);
    SG_PROP_USER = Register(textProp, "US", fInfoProp + fNewLine);
    SG_PROP_TIME = Register(textProp, "TM", fInfoProp);
    SG_PROP_SOURCE = Register(textProp, "SO", fInfoProp + fNewLine);
    SG_PROP_COPYRIGHT = Register(textProp, "CP", fInfoProp);
    SG_PROP_ANALYSIS = Register(textProp, "AN", fInfoProp);
    SG_PROP_RANK_BLACK = Register(textProp, "BR", fInfoProp + fBlackProp);
    SG_PROP_RANK_WHITE = Register(textProp, "WR", fInfoProp + fWhiteProp);
    SG_PROP_TEAM_BLACK = Register(textProp, "BT", fInfoProp + fBlackProp);
    SG_PROP_TEAM_WHITE = Register(textProp, "WT", fInfoProp + fWhiteProp);
    SG_PROP_OPENING = Register(textProp, "ON", fInfoProp + fNewLine);
    SG_PROP_RULES = Register(textProp, "RU", fInfoProp + fNewLine);
    SG_PROP_HANDICAP = Register(intProp, "HA", fInfoProp);
    SG_PROP_KOMI = Register(realProp, "KM", fInfoProp);

    //--- abstract properties
    SG_PROP_FIND_MOVE = Register(0, "", fAbstract);
    SG_PROP_FIND_TEXT = Register(0, "", fAbstract);
    SG_PROP_BRANCH = Register(0, "", fAbstract);
    SG_PROP_TERMINAL = Register(0, "", fAbstract);

    //--- Smart Go specific properties
    SG_PROP_MOTIVE =
        Register(textListProp, "MM", fStatProp + fCustom + fNotClean);
    SG_PROP_SEQUENCE =
        Register(listProp, "MS", fStatProp + fCustom + fNotClean);
    SG_PROP_NOT_EMPTY =
        Register(listProp, "NE", fMarkProp + fCustom + fNotClean);
    SG_PROP_NOT_BLACK =
        Register(listProp, "NB", fMarkProp + fCustom + fNotClean);
    SG_PROP_NOT_WHITE =
        Register(listProp, "NW", fMarkProp + fCustom + fNotClean);

    s_initialized = true;
}

void SgProp::Fini()
{
    s_initialized = false;
}

//----------------------------------------------------------------------------

SgProp* SgPropUnknown::Duplicate() const
{
    return new SgPropUnknown(m_id, m_label, m_values);
}

bool SgPropUnknown::ToString(std::vector<std::string>& values, int boardSize,
                             SgPropPointFmt fmt, int fileFormat) const
{
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    SG_UNUSED(fileFormat);
    values.clear();
    for (vector<string>::const_iterator it = m_values.begin();
         it != m_values.end(); ++it)
        values.push_back( EscapeSpecialCharacters(*it, false));
    return true;
}

bool SgPropUnknown::FromString(const std::vector<std::string>& values,
                               int boardSize, SgPropPointFmt fmt)
{
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    m_values = values;
    return true;
}

//----------------------------------------------------------------------------

SgProp* SgPropSimple::Duplicate() const
{
    return new SgPropSimple(m_id);
}

bool SgPropSimple::ToString(std::vector<std::string>& values, int boardSize,
                            SgPropPointFmt fmt, int fileFormat) const
{
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    SG_UNUSED(fileFormat);
    values.assign(1, "");
    return true;
}

bool SgPropSimple::FromString(const std::vector<std::string>& values,
                                int boardSize, SgPropPointFmt fmt)
{
    SG_UNUSED(values);
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    return true;
}

//----------------------------------------------------------------------------

SgProp* SgPropMultiple::Duplicate() const
{
    return new SgPropMultiple(m_id, m_value);
}

//----------------------------------------------------------------------------

SgProp* SgPropInt::Duplicate() const
{
    return new SgPropInt(m_id, m_value);
}

bool SgPropInt::ToString(std::vector<std::string>& values, int boardSize,
                         SgPropPointFmt fmt, int fileFormat) const
{
    SG_UNUSED(fileFormat);
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    ostringstream buffer;
    buffer << m_value;
    values.assign(1, buffer.str());
    return true;
}

bool SgPropInt::FromString(const std::vector<std::string>& values,
                           int boardSize, SgPropPointFmt fmt)
{
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    if (values.empty())
    {
        m_value = 0;
        return true;
    }
    istringstream in(values[0]);
    in >> m_value;
    return (! in.fail());
}

//----------------------------------------------------------------------------

SgProp* SgPropReal::Duplicate() const
{
    return new SgPropReal(m_id, m_value, m_precision);
}

bool SgPropReal::ToString(std::vector<std::string>& values, int boardSize,
                          SgPropPointFmt fmt, int fileFormat) const
{
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    SG_UNUSED(fileFormat);
    ostringstream buffer;
    buffer << fixed << setprecision(m_precision) << m_value;
    values.assign(1, buffer.str());
    return true;
}

bool SgPropReal::FromString(const std::vector<std::string>& values,
                            int boardSize, SgPropPointFmt fmt)
{
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    if (values.empty())
    {
        m_value = 0;
        return true;
    }
    istringstream in(values[0]);
    in >> m_value;
    return (! in.fail());
}

//----------------------------------------------------------------------------

void SgPropValue::ChangeToOpponent()
{
    m_value = -m_value;
}

SgProp* SgPropValue::Duplicate() const
{
    return new SgPropValue(m_id, m_value);
}

//----------------------------------------------------------------------------

SgPropTime::~SgPropTime()
{
}

SgProp* SgPropTime::Duplicate() const
{
    return new SgPropTime(m_id, m_value, m_precision);
}

//----------------------------------------------------------------------------

SgPropMSec::~SgPropMSec()
{
}

SgProp* SgPropMSec::Duplicate() const
{
    return new SgPropMSec(m_id, m_value);
}

//----------------------------------------------------------------------------

SgPropVoidList::~SgPropVoidList()
{
}

SgProp* SgPropVoidList::Duplicate() const
{
    return new SgPropVoidList(m_id, m_list);
}

bool SgPropVoidList::ToString(std::vector<std::string>& values, int boardSize,
                                  SgPropPointFmt fmt, int fileFormat) const
{
    SG_UNUSED(values);
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    SG_UNUSED(fileFormat);
    return false;
}

bool SgPropVoidList::FromString(const std::vector<std::string>& values,
                                  int boardSize, SgPropPointFmt fmt)
{
    SG_UNUSED(values);
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    return true;
}

//----------------------------------------------------------------------------

SgPropPointList::~SgPropPointList()
{
}

SgProp* SgPropPointList::Duplicate() const
{
    return new SgPropPointList(ID(), Value());
}

bool SgPropPointList::ToString(std::vector<std::string>& values,
                               int boardSize, SgPropPointFmt fmt,
                               int fileFormat) const
{
    // Don't write out empty list properties.
    if (Value().IsEmpty())
        return false;
    values.clear();
    for (SgListIterator<SgPoint> it(Value()); it; ++it)
    {
        if (! IsSpecialMove(*it))
            values.push_back(PointToSgfString(*it, boardSize, fmt,
                                              fileFormat));
    }
    return true;
}

bool SgPropPointList::FromString(const std::vector<std::string>& values,
                                 int boardSize, SgPropPointFmt fmt)
{
    for (vector<string>::const_iterator it = values.begin();
         it != values.end(); ++it)
    {
        string s = *it;
        if (s.size() == 5 && s[2] == ':')
        {
            // Compressed point list.
            string s1 = s.substr(0, 2);
            string s2 = s.substr(3, 2);
            SgPoint p1 = SgfStringToPoint(s1, boardSize, fmt);
            SgPoint p2 = SgfStringToPoint(s2, boardSize, fmt);
            if (InBoardRange(p1) && InBoardRange(p2))
            {
                SgRect rect(p1, p2);
                for (SgRectIterator iter(rect); iter; ++iter)
                    Value().Append(*iter);
            }
            else
            {
                return false;
            }
        }
        else
        {
            // Single point.
            SgPoint p = SgfStringToPoint(s, boardSize, fmt);
            if (InBoardRange(p) || p == SG_PASS)
                // Pass needed for move sequence
                Value().Append(p);
            else
                return false;
        }
    }
    return true;
}

//----------------------------------------------------------------------------

SgProp* SgPropText::Duplicate() const
{
    return new SgPropText(m_id, m_text);
}

bool SgPropText::ToString(std::vector<std::string>& values, int boardSize,
                          SgPropPointFmt fmt, int fileFormat) const
{
    SG_UNUSED(fileFormat);
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    values.assign(1, EscapeSpecialCharacters(m_text, false));
    return true;
}

bool SgPropText::FromString(const std::vector<std::string>& values,
                              int boardSize, SgPropPointFmt fmt)
{
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    if (values.size() == 0)
        m_text = "";
    else
        m_text = values[0];
    return true;
}

bool SgPropText::ContainsText(const string& findText)
{
    return (m_text.find(findText) != string::npos);
}

//----------------------------------------------------------------------------

SgPropTextList::SgPropTextList(SgPropID id, const SgList<SgPoint>& points,
                               SgListOf<std::string> strings)
    : SgProp(id),
      m_points(points),
      m_strings()
{
    for (SgListIteratorOf<string> it(strings); it; ++it)
    {
        m_strings.Append(new string(*(*it)));
    }
}

SgPropTextList::~SgPropTextList()
{
    for (SgListIteratorOf<string> it(m_strings); it; ++it)
    {
        delete *it;
    }
}

SgProp* SgPropTextList::Duplicate() const
{
    return new SgPropTextList(m_id, m_points, m_strings);
}

bool SgPropTextList::GetStringAtPoint(SgPoint p, string* s) const
{
    int index = m_points.Index(p);
    if (index > 0)
    {
        *s = *m_strings[index];
        return true;
    }
    return false;
}

void SgPropTextList::AddStringAtPoint(SgPoint p, const string& s)
{
    ClearStringAtPoint(p);
    m_points.Append(p);
    m_strings.Append(new string(s));
}

void SgPropTextList::AppendToStringAtPoint(SgPoint p, const string& s)
{
    string value;
    GetStringAtPoint(p, &value); // may be empty.
    value += s;
    AddStringAtPoint(p, value); // this clears out old string, if any.
}

void SgPropTextList::ClearStringAtPoint(SgPoint p)
{
    int index = m_points.Index(p);
    if (index > 0)
    {
        m_points.DeleteAt(index);
        delete m_strings[index];
        m_strings.DeleteAt(index);
    }
}

bool SgPropTextList::ToString(std::vector<std::string>& values,
                              int boardSize, SgPropPointFmt fmt,
                              int fileFormat) const
{
    // Don't write out empty text list properties.
    if (m_points.IsEmpty())
        return false;
    values.clear();
    int index = 0;
    for (SgListIterator<SgPoint> iter(m_points); iter; ++iter)
    {
        ostringstream buffer;
        buffer << PointToSgfString(*iter, boardSize, fmt, fileFormat) << ':'
               << EscapeSpecialCharacters(*m_strings[++index], true);
        values.push_back(buffer.str());
    }
    return true;
}

bool SgPropTextList::FromString(const std::vector<std::string>& values,
                                  int boardSize, SgPropPointFmt fmt)
{
    for (vector<string>::const_iterator it = values.begin();
         it != values.end(); ++it)
    {
        string s = *it;
        if (s.size() >= 2)
        {
            string pointString = s.substr(0, 2);
            SgPoint p = SgfStringToPoint(pointString, boardSize, fmt);
            if (InBoardRange(p) && s.size() >= 3 && s[2] == ':')
            {
                string text = s.substr(3);
                AddStringAtPoint(p, text);
                return true;
            }
        }
    }
    return false;
}

bool SgPropTextList::ContainsText(const string& findText)
{
    for (SgListIteratorOf<string> it(m_strings); it; ++it)
    {
        if ((*it)->find(findText) != string::npos)
            return true;
    }
    return false;
}

//----------------------------------------------------------------------------

SgProp* SgPropPlayer::Duplicate() const
{
    return new SgPropPlayer(m_id, m_player);
}

bool SgPropPlayer::ToString(std::vector<std::string>& values, int boardSize,
                            SgPropPointFmt fmt, int fileFormat) const
{
    SG_UNUSED(fileFormat);
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    values.assign(1, m_player == SG_WHITE ? "W" : "B");
    return true;
}

bool SgPropPlayer::FromString(const std::vector<std::string>& values,
                              int boardSize, SgPropPointFmt fmt)
{
    SG_UNUSED(boardSize);
    SG_UNUSED(fmt);
    if (values.size() == 0)
        return false;
    m_player = (values[0] == "W" || values[0] == "w") ? SG_WHITE : SG_BLACK;
    return true;
}

void SgPropPlayer::ChangeToOpponent()
{
    m_player = SgOppBW(m_player);
}

//----------------------------------------------------------------------------

SgPropAddStone::~SgPropAddStone()
{
}

SgProp* SgPropAddStone::Duplicate() const
{
    return new SgPropAddStone(m_id, Value());
}

//----------------------------------------------------------------------------

SgProp* SgPropMove::Duplicate() const
{
    return new SgPropMove(m_id, m_move);
}

bool SgPropMove::ToString(std::vector<std::string>& values, int boardSize,
                          SgPropPointFmt fmt, int fileFormat) const
{
    values.assign(1, PointToSgfString(m_move, boardSize, fmt, fileFormat));
    return true;
}

bool SgPropMove::FromString(const std::vector<std::string>& values,
                              int boardSize, SgPropPointFmt fmt)
{
    if (values.size() == 0)
        return false;
    m_move = SgfStringToPoint(values[0], boardSize, fmt);
    return m_move != SG_NULLMOVE;
}

//----------------------------------------------------------------------------

