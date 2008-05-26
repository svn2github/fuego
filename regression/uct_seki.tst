#-----------------------------------------------------------------------------
# Seki tests for GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000

#-----------------------------------------------------------------------------

loadsgf sgf/games/2007/seki-evaluation-problem.sgf 53
reg_genmove b
10 sg_compare_float 0.3 uct_value
#? [-1]*

loadsgf sgf/games/2007/seki-evaluation-problem.sgf 54
reg_genmove w
20 sg_compare_float 0.7 uct_value
#? [1]*

loadsgf sgf/games/2007/CGOS/193838.sgf 80
30 reg_genmove w
#? [J1]*
# kill seki by ko fight to win this game.

40 sg_compare_float 0.7 uct_value
#? [1]*

loadsgf sgf/games/2007/CGOS/193838.sgf 81
50 reg_genmove b
#? [J6]
# make seki. Only chance to win this game.

60 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/games/2007/CGOS/193838.sgf 82
70 reg_genmove w
#? [J1]*
# kill seki by ko fight to win this game.

80 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/games/2008/leela-seki-variation.sgf 45
90 reg_genmove b
#? [A8]
# kill, avoid seki.

100 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/games/2008/leela-seki-variation.sgf 46
110 reg_genmove w
#? [A8]
# make seki.

120 sg_compare_float 0.5 uct_value
#? [1]*
# cannot see the win for a very long time

loadsgf sgf/games/2008/leela-seki-variation.sgf 48
130 reg_genmove w
#? [A7]
# make seki.

140 sg_compare_float 0.5 uct_value
#? [1]*
# cannot see the win for a very long time

loadsgf sgf/games/2008/leela-seki-variation.sgf 58
reg_genmove w
150 sg_compare_float 0.5 uct_value
#? [1]*
# cannot see the win for a very long time

loadsgf sgf/games/2008/leela-seki-variation.sgf 64
reg_genmove w
160 sg_compare_float 0.5 uct_value
#? [1]*
# cannot see the win for a very long time

loadsgf sgf/games/2008/leela-seki-variation.sgf 72
reg_genmove w
170 sg_compare_float 0.5 uct_value
#? [1]*
# cannot see the win for a very long time

loadsgf sgf/games/2008/leela-seki-variation.sgf 76
reg_genmove w
180 sg_compare_float 0.5 uct_value
#? [1]*
# cannot see the win for a very long time

loadsgf sgf/games/2008/leela-seki-variation.sgf 78
reg_genmove w
190 sg_compare_float 0.5 uct_value
#? [1]
# finally!

loadsgf sgf/games/2008/seki-miseval.sgf 37
reg_genmove b
200 sg_compare_float 0.5 uct_value
#? [-1]*
# B is totally lost because the top corner is seki.
# in playouts, the B selfataries get moved to the other liberty at J7
# but the W selfatari is eventually played.

loadsgf sgf/games/2008/seki-miseval.sgf 47
reg_genmove b
210 sg_compare_float 0.5 uct_value
#? [-1]*

loadsgf sgf/games/2008/seki-miseval.sgf 57
reg_genmove b
220 sg_compare_float 0.5 uct_value
#? [-1]*

loadsgf sgf/games/2008/seki-miseval.sgf 59
reg_genmove b
230 sg_compare_float 0.5 uct_value
#? [-1]
# finally!
