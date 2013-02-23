#-----------------------------------------------------------------------------
# Long ladder blunders from Fuego 19x19 games. 
# Capture or escape blunders that could be avoided by reading a ladder.
#-----------------------------------------------------------------------------

loadsgf sgf/games/2008/CGOS/31425.sgf 90
10 reg_genmove white
#? [!L12]

loadsgf sgf/games/2008/CGOS/31425.sgf 92
20 reg_genmove white
#? [!M14]*

loadsgf sgf/games/2008/CGOS/31425.sgf 94
30 reg_genmove white
#? [!K13]*

loadsgf sgf/ladder/ladder-blunder.sgf 32
40 reg_genmove white
#? [!G6]*
# testing only a few moves, not all the bad ladder

loadsgf sgf/ladder/ladder-blunder.sgf 32
50 reg_genmove white
#? [!G6]*

loadsgf sgf/ladder/ladder-blunder.sgf 34
60 reg_genmove white
#? [!H7]*
# testing only a few moves, not all the bad ladder

loadsgf sgf/ladder/ladder-blunder.sgf 42
70 reg_genmove white
#? [!K9]*

loadsgf sgf/ladder/ladder-blunder.sgf 52
80 reg_genmove white
#? [!O10]*
