#-----------------------------------------------------------------------------
# Test two liberty policies.
#-----------------------------------------------------------------------------

uct_param_player search_mode playout_policy

#-----------------------------------------------------------------------------

loadsgf sgf/playout/playout_two_liberties_throw_in.sgf 2
10 reg_genmove w
#? [J3]*
# must throw in to win semeai.

loadsgf sgf/playout/playout_two_liberties_throw_in.sgf 4
20 reg_genmove w
#? [J3]*
# must throw in to win semeai.

loadsgf sgf/playout/playout_two_liberties_throw_in.sgf 6
30 reg_genmove w
#? [J3]*
# capture.

