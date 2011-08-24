#-----------------------------------------------------------------------------
# Test playout policy on nakade shapes.
#-----------------------------------------------------------------------------

uct_param_player search_mode playout_policy

#-----------------------------------------------------------------------------

loadsgf sgf/playout/playout_nakade.1.sgf 3
10 reg_genmove w
#? [J8]
# must make nakade shape.

11 uct_policy_moves
#? [AtariCapture J8]
# todo: AtariCapture is kind of a silly reason

loadsgf sgf/playout/playout_nakade.1.sgf 5
20 reg_genmove w
#? [H8]*
# must kill on vital point.

21 uct_policy_moves
#? [Nakade H8]*
# Nakade policy does not exist yet.

loadsgf sgf/playout/playout_nakade.1.sgf 7
30 reg_genmove w
#? [J8]
# must kill on vital point.

31 uct_policy_moves
#? [Pattern J8]
# Should be different move reason in future ???