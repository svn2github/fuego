#-----------------------------------------------------------------------------
# Semeai tests for GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000

#-----------------------------------------------------------------------------

loadsgf sgf/games/2007/CGOS/196322-variation.2.sgf 45
10 reg_genmove b
#? [F3|H3|H5]*
# b can get seki or a favorable ko in this corner and win.
# 20080126 run: new FAIL.

20 sg_compare_float 0.5 uct_value
#? [1]
# B wins but the program does not understand why.

loadsgf sgf/games/2007/CGOS/196322-variation.2.sgf 47
reg_genmove b
30 sg_compare_float 0.3 uct_value
#? [-1]*
# B is lost but does not see it.

loadsgf sgf/games/2007/CGOS/196322-variation.2.sgf 49
reg_genmove b
40 sg_compare_float 0.3 uct_value
#? [-1]*
# B is lost but does not see it.

loadsgf sgf/games/2007/CGOS/196322-variation.1.sgf 33
50 reg_genmove b
#? [H5|G6|J6]
# H5 is simpler but it seems G6 also works - see 196322-variation.2.sgf
# J6 is probably OK but I did not research it.

loadsgf sgf/games/2007/CGOS/196322-variation.1.sgf 35
60 reg_genmove b
#? [J4|F3|G1|G4]
# maybe even more moves work.

loadsgf sgf/games/2007/CGOS/194557.sgf 43
70 reg_genmove b
#? [G5|G1]
# H3 loses quickly. G5 is much more complicated, and probably good for B.
# G1 probably works but did not research in detail.

loadsgf sgf/games/2007/CGOS/200109.sgf 27
80 reg_genmove b
#? [G8]*
# does not work against strongest response F8 G9 F7, but best try by far.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 29
90 reg_genmove b
#? [G8]*
# strengthened B on left side to make this into a good semeai problem.

100 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 31
110 reg_genmove b
#? [G9]*
# It loves G2 but that is wrong.

120 sg_compare_float 0.5 uct_value
#? [1]*
# B can win.
# 20080126 run: new FAIL.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 33
130 reg_genmove b
#? [F9]

140 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 35
150 reg_genmove b
#? [D8]

160 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 37
170 reg_genmove b
#? [C8]

180 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 39
190 reg_genmove b
#? [D9]

200 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2007/CGOS/200109-variation.sgf 41
210 reg_genmove b
#? [B8]

220 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2008/CGOS/249009.sgf 59
230 reg_genmove b
#? [C9]*
# make eye to make seki, win game

240 sg_compare_float 0.5 uct_value
#? [1]

loadsgf sgf/games/2008/CGOS/248559.sgf 47
250 reg_genmove b
#? [G1]
# win semeai - good ko for us

260 sg_compare_float 0.5 uct_value
#? [1]
# B can win.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 36
270 reg_genmove w
#? [A7|A8|A9|G9]*
# win semeai - 3 vs 3 liberties

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 37
280 reg_genmove b
#? [G9]*

290 sg_compare_float 0.6 uct_value
#? [1]*
# B can win.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 38
reg_genmove w
300 sg_compare_float 0.4 uct_value
#? [-1]*
# W lost.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 39
310 reg_genmove b
#? [E7|G7|J6]*

320 sg_compare_float 0.6 uct_value
#? [1]*
# B can win.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 40
reg_genmove w
330 sg_compare_float 0.4 uct_value
#? [-1]*
# W lost.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 41
340 reg_genmove b
#? [G7|J6]*

350 sg_compare_float 0.6 uct_value
#? [1]*
# B can win.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 42
reg_genmove w
360 sg_compare_float 0.4 uct_value
#? [-1]*
# W lost.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 43
370 reg_genmove b
#? [G7|A8]*

380 sg_compare_float 0.6 uct_value
#? [1]*
# B can win.

loadsgf sgf/games/2008/CGOS/uct20080117031720-variation.sgf 44
reg_genmove w
390 sg_compare_float 0.05 uct_value
#? [-1]
# W lost.

loadsgf sgf/semeai/semeai-eval.1.sgf 1
400 reg_genmove b
#? [A4]

410 sg_compare_float 0.6 uct_value
#? [1]*
# B won, but current program is unsure about bottom right - score around 0.5.

loadsgf sgf/semeai/semeai-eval.1.sgf 3
420 reg_genmove b
#? [H2]

430 sg_compare_float 0.6 uct_value
#? [1]*
# B won, but current program is unsure about bottom right - score around 0.5.

loadsgf sgf/semeai/semeai-eval.1.sgf 5
440 reg_genmove b
#? [E3]

450 sg_compare_float 0.6 uct_value
#? [1]*
# B won, but current program is unsure about bottom right - score around 0.5.

loadsgf sgf/semeai/semeai-eval.1.sgf 7
460 reg_genmove b
#? [J2]

470 sg_compare_float 0.6 uct_value
#? [1]*
# B won, but current program is unsure about bottom right - score around 0.5.
