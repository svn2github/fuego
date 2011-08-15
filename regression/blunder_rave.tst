#-----------------------------------------------------------------------------
# Test cases where RAVE blunders.
#-----------------------------------------------------------------------------

uct_param_search rave 1

#-----------------------------------------------------------------------------

loadsgf sgf/rave-problems/823341-variation-rave-bug.sgf 
10 reg_genmove w
#? [B2]*
# W needs the territory here, loses if this becomes seki.

uct_param_search rave 0
11 reg_genmove w
#? [B2]
uct_param_search rave 1

loadsgf sgf/rave-problems/results-105.sgf 72
20 reg_genmove w
#? [H9]*
# needs approach move.

uct_param_search rave 0
21 reg_genmove w
#? [H9]
uct_param_search rave 1

loadsgf sgf/games/2011/KGS-74-August2011-9x9-fast/pachi2-Fuego-2.sgf 69
30 reg_genmove b
#? [B2]*
# B2 is jigo. This case is very similar to 823341-variation-rave-bug.sgf, 
# but not solved even with rave off

uct_param_search rave 0
31 reg_genmove w
#? [B2]*
uct_param_search rave 1
