#-----------------------------------------------------------------------------
# Tests for opening book of GoUctGlobalSearchPlayer.
#-----------------------------------------------------------------------------

uct_param_player max_games 50000
book_load ../book/book.dat

#-----------------------------------------------------------------------------
loadsgf sgf/games/2007/CGOS/200368.sgf 9
10 reg_genmove b
#? [D2]
# E2 seems OK too.

loadsgf sgf/games/2007/CGOS/193991.sgf 2
20 reg_genmove w
#? [E4]

