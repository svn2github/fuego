#-----------------------------------------------------------------------------
# Blunder avoidance tests for GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000
komi 7.5

#-----------------------------------------------------------------------------

loadsgf sgf/games/2007/CGOS/191282.sgf 10
10 reg_genmove w
#? [C7]
# program passed in the game...

loadsgf sgf/games/2007/CGOS/191282.sgf 32
20 reg_genmove w
#? [J1]
# connecting gives safe win

loadsgf sgf/games/2007/CGOS/147759.sgf 51
30 reg_genmove b
#? [E9|J3|D4]
# B needs to make 1 point to win. In the game it played a dame and lost.

loadsgf sgf/games/2007/CGOS/71459.sgf 54
40 reg_genmove w
#? [H5|J1|A6]
# F5 was selfatari

loadsgf sgf/games/2007/CGOS/71459.sgf 56
50 reg_genmove w
#? [G5]
# recapture

loadsgf sgf/games/2007/CGOS/107433.sgf 18
60 reg_genmove w
#? [C6|G7|H7]
# the game move B4 is a blunder, in own territory

loadsgf sgf/games/2008/gnu-uct3K-11.sgf 56
70 reg_genmove w
#? [A1]*
# must capture now to win by 0.5. Often the simulatons do not see that B
# has a ko threat, so they waste a move and play inside territory.
# Actually, W can still win - see next test - but capturing now 
# is so much safer.

loadsgf sgf/games/2008/gnu-uct3K-11.sgf 62
80 reg_genmove w
#? [A7]
# must play this local ko threat to win.

loadsgf sgf/games/2008/blunder-move-25.sgf 21
90 reg_genmove b
#? [D6]*
# not blunder yet, but capturing right away is simplest win.

loadsgf sgf/games/2008/blunder-move-25.sgf 25
100 reg_genmove b
#? [D6]*
# must play this capture and fight to win.
# takes over 100000 simulations to become best move,
# and value is still very low - about 0.24.

loadsgf sgf/games/2008/blunder-move-25.sgf 27
110 reg_genmove b
#? [C7]*

loadsgf sgf/games/2008/blunder-move-25.sgf 29
120 reg_genmove b
#? [E1|B1]*
# don't think C1 also works as ko threat

loadsgf sgf/games/2008/blunder-move-25.sgf 33
130 reg_genmove b
#? [C8|B6]*

loadsgf sgf/games/2008/blunder-move-25.sgf 35
140 reg_genmove b
#? [B6]*

loadsgf sgf/games/2008/blunder-move-25.sgf 33
150 reg_genmove b
#? [B8|A8]*
# see comments in sgf file on A8

loadsgf sgf/games/2008/gnu-uct3K-17.sgf 58
160 reg_genmove w
#? [A2]*
# with few simulations it likes throw-in A1, loses because no ko threats
# A2 is trivial 0.5 pt win.

loadsgf sgf/games/2008/CGOS/386221.sgf 63
170 reg_genmove b
#? [G9]
