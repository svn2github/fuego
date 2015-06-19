#-----------------------------------------------------------------------------
# Test playout policy on basic semeai.
#-----------------------------------------------------------------------------

uct_param_player search_mode playout_policy

#-----------------------------------------------------------------------------

loadsgf sgf/semeai/playout_semeai.1.sgf
10 reg_genmove w
#? [C1]*
# 2 vs 2

loadsgf sgf/semeai/playout_semeai.2.sgf
20 reg_genmove w
#? [!(D9|F6|J6)]
# 4 vs 2 eye vs no eye, do not fill own libs

loadsgf sgf/semeai/playout_semeai.3.sgf
30 reg_genmove b
#? [D9|J6]*
# 3 vs 2 eye vs no eye, fill lib

loadsgf sgf/semeai/playout_semeai.3.sgf
40 reg_genmove b
#? [!J9]*
# 3 vs 2 eye vs no eye, fill lib
