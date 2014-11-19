#-----------------------------------------------------------------------------
# Test cases from games where Fuego missed an urgent move
#-----------------------------------------------------------------------------

loadsgf sgf/games/2014/Fuego-GnuGo-2014-10-26.sgf 36
10 reg_genmove w
#? [Q8|R8]*

loadsgf sgf/games/2014/Fuego-GnuGo-2014-10-26.sgf 38
20 reg_genmove w
#? [R7]*

loadsgf sgf/games/2014/Fuego-GnuGo-2014-10-26.sgf 40
30 reg_genmove w
#? [S6]*

loadsgf sgf/games/2014/Fuego-GnuGo-2014-10-26.sgf 42
40 reg_genmove w
#? [S1|T2|S5]
# connecting at R5 here risks the whole corner.
# S5 might be possible, a complex ko.

