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

31 uct_policy_moves_simple
#? [GammaPattern J8]
# Should be different move reason in future ???

loadsgf sgf/playout/playout_nakade.2.sgf
40 uct_policy_moves
#? [GammaPattern E1:1 F1:550.309]
# F1 is much more likely, but E1 would be fatal...

41 uct_policy_moves_simple
#? [GammaPattern F1]*