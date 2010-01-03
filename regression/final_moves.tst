#-----------------------------------------------------------------------------
# Tests related to final moves in a game such as capturing dead stones,
# approach moves, inside connection moves, filling neutral points etc.
#-----------------------------------------------------------------------------

loadsgf sgf/games/2010/CGOS/1005347.sgf

10 reg_genmove b
#? [A6|B6|H1|J1]*
# Fuego resigns here because the search gets stuck on the sequence PASS PASS
