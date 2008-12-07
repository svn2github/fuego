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
#? [J1]
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
#? [J1]
# kill seki by ko fight to win this game.

80 sg_compare_float 0.7 uct_value
#? [1]*

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
#? [A7]*
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
#? [-1]

loadsgf sgf/games/2008/seki-miseval.sgf 59
reg_genmove b
230 sg_compare_float 0.5 uct_value
#? [-1]
# finally!

loadsgf sgf/games/2008/KGS/7/26/Newou-Fuego9.sgf 31
240 reg_genmove b
#? [F4]
# kill, avoid seki.

loadsgf sgf/games/2008/KGS/7/26/Newou-Fuego9.sgf 35
250 reg_genmove b
#? [D4]*
# already is seki, F4 is bad attack and loses the game.

loadsgf sgf/games/2008/KGS/7/26/Newou-Fuego9.sgf 41
reg_genmove b
260 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/KGS/7/26/Newou-Fuego9.sgf 53
reg_genmove b
270 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/KGS/7/25/PaperTiger-Fuego.sgf 163
280 reg_genmove b
#? [Q13]
# Q13 is the only way to attack. O12 leads to seki.

loadsgf sgf/games/2008/KGS/7/25/PaperTiger-Fuego.sgf 235
reg_genmove b
290 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/KGS/7/25/PaperTiger-Fuego.sgf 345
reg_genmove b
300 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/KGS/7/28/dege-Fuego9.sgf 31
310 reg_genmove b
#? [F1]*
# F1 kills, prevents seki.

loadsgf sgf/games/2008/KGS/7/28/dege-Fuego9.sgf 37
reg_genmove b
320 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 1
330 reg_genmove b
#? [G9]*
# prevents seki.
# This position was analyzed by "bitti" in the German Go Bund forum
# http://www.dgob.de/yabbse/index.php?topic=3314.msg153446#msg153446

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 3
340 reg_genmove b
#? [J8]*

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 5
350 reg_genmove b
#? [E9|B9|A8]*
# E9 is good style, but others work too.

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 7
360 reg_genmove b
#? [J7|B9|A8]*

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 9
370 reg_genmove b
#? [B9|A8]*

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 11
380 reg_genmove b
#? [A8]*

loadsgf sgf/games/2008/KGS/8/17/AyaMC-Fuego9-variation.sgf 13
390 reg_genmove b
#? [A5]*

loadsgf sgf/games/2008/CGOS/seki-miseval-20081113151939.sgf 2
reg_genmove b
400 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/CGOS/seki-miseval-20081113151939.sgf 4
reg_genmove b
410 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/CGOS/seki-miseval-20081113151939.sgf 6
reg_genmove b
420 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/CGOS/seki-miseval-20081113151939.sgf 8
reg_genmove b
430 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/CGOS/seki-miseval-20081113151939.sgf 10
reg_genmove b
440 sg_compare_float 0.5 uct_value
#? [-1]*
# B is losing but thinks it wins the seki.

loadsgf sgf/games/2008/CGOS/Fuego-640-8c-20081113151939.sgf 59
450 reg_genmove b
#? [E9]*
# E9 wins the endgame. Because of seki misevaluation, B plays 'safe'
# and loses this game.
