#-----------------------------------------------------------------------------
# Test cases where playout policy should generate an approach move.
#-----------------------------------------------------------------------------

uct_param_player search_mode playout_policy

#-----------------------------------------------------------------------------

loadsgf sgf/playout/playout_nakade_approach.sgf
10 reg_genmove w
#? [J5]*
# must connect first to win semeai.

