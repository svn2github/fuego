#-----------------------------------------------------------------------------
# Blunder avoidance tests for endgames.
#-----------------------------------------------------------------------------

loadsgf sgf/games/2011/KGS-74-August2011-9x9-fast/pachi2-Fuego-2.sgf 49
10 reg_genmove b
#? [F9|G1]

loadsgf sgf/games/2011/KGS-74-August2011-9x9-fast/pachi2-Fuego-2.sgf 57
20 reg_genmove b
#? [J5]*

loadsgf sgf/games/2011/KGS-74-August2011-9x9-fast/pachi2-Fuego-2.sgf 59
30 reg_genmove b
#? [J5]

loadsgf sgf/games/2011/KGS-74-August2011-9x9-fast/pachi2-Fuego-2.sgf 63
40 reg_genmove b
#? [J5]
# J5 is 1 point and wins. F5 is dame, leads to jigo

loadsgf sgf/games/2011/KGS-74-August2011-9x9-fast/pachi2-Fuego-2.sgf 69
50 reg_genmove b
#? [B2]*
# B2 is jigo
