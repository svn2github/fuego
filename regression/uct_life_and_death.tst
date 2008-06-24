#-----------------------------------------------------------------------------
# Life and Death tests for GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000

#-----------------------------------------------------------------------------

loadsgf sgf/games/2008/CGOS/248520.sgf 25
10 reg_genmove b
#? [J4|H5]*
# keeping w separated is easiest way to keep both sides dead

20 sg_compare_float 0.5 uct_value
#? [1]
# B wins.

loadsgf sgf/games/2008/CGOS/248520.sgf 31
30 reg_genmove b
#? [J4|H5]*
# keeping w separated, kill top, is enough to win game

40 sg_compare_float 0.5 uct_value
#? [1]
# B wins.

loadsgf sgf/games/2008/CGOS/248520.sgf 43
50 reg_genmove b
#? [J4]*
# keeping w separated, kill one side, is enough to win game

60 sg_compare_float 0.5 uct_value
#? [1]
# B wins.

loadsgf sgf/games/2008/mogo-ko+ld-problem-variation.sgf 35
reg_genmove w
70 sg_compare_float 0.5 uct_value
#? [-1]*
# W lost, but program does not see it
# Only after Black E2 the value drops

loadsgf sgf/games/2008/mogo-ko+ld-problem-variation.sgf 37
reg_genmove w
80 sg_compare_float 0.2 uct_value
#? [-1]
# After Black E2 the program sees the loss

loadsgf sgf/games/2008/mogo-ko+ld-problem.sgf
90 reg_genmove w
#? [J4]
# apparently Mogo cannot solve this one.

loadsgf sgf/lifeanddeath/4x4nakade.sgf
reg_genmove w
100 sg_compare_float 0.8 uct_value
#? [1]
# apparently Mogo has some trouble with this one, thinks the b group is alive.

loadsgf sgf/lifeanddeath/eye-tricky.sgf 1
110 reg_genmove b
#? [D3]
# only way to live.

120 sg_compare_float 0.9 uct_value
#? [1]

130 reg_genmove w
#? [D3|F1|F3]
# three ways to kill.

140 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/lifeanddeath/eye-tricky.sgf 2
150 reg_genmove w
#? [D3]

160 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/lifeanddeath/eye-tricky.sgf 4
170 reg_genmove w
#? [E2]

180 sg_compare_float 0.7 uct_value
#? [1]

loadsgf sgf/lifeanddeath/eye-tricky.sgf 6
190 reg_genmove w
#? [F3]

200 sg_compare_float 0.9 uct_value
#? [1]

loadsgf sgf/games/2008/gnu-uct3K-28.sgf 40
210 reg_genmove w
#? [G8|H9]

