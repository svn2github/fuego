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


