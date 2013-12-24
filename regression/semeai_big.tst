#-----------------------------------------------------------------------------
# Tests with large Semeai.
#-----------------------------------------------------------------------------

loadsgf sgf/games/2011/KGS-January/PueGo-pachi2.sgf 190
10 reg_genmove w
#? [M13]*
# extend to avoid a lost semeai.

loadsgf sgf/games/2011/KGS-January/PueGo-pachi2.sgf 191
20 reg_genmove b
#? [M13]*
# surround and win semeai.

loadsgf sgf/games/2011/KGS/CzechBot-Zen19S.sgf 183
30 reg_genmove b
#? [A8|A10]
# Actually, B could tenuki and win the semeai. But the move L18 in the game
# fills its own liberty.

31 reg_genmove b
#? [!L18]

loadsgf sgf/games/2011/KGS/CzechBot-Zen19S.sgf 184
40 reg_genmove w
#? [K19|L11|M10|N15]*

loadsgf sgf/games/2011/KGS/CzechBot-Zen19S-semeai.sgf 2
50 reg_genmove b
#? [A8|A10]*

loadsgf sgf/games/2011/KGS/CzechBot-Zen19S-semeai.sgf 4
60 reg_genmove b
#? [A10]*

loadsgf sgf/games/2011/KGS/CzechBot-Zen19S-semeai.sgf 6
70 reg_genmove b
#? [C9]*

loadsgf sgf/games/2011/KGS/CzechBot-Zen19S-semeai.sgf 8
80 reg_genmove b
#? [A9]*

loadsgf sgf/games/2011/KGS/CzechBot-Zen19S-semeai.sgf 10
90 reg_genmove b
#? [C9]

loadsgf sgf/games/2011/KGS/CzechBot-Zen19S-semeai.sgf 12
100 reg_genmove b
#? [D8]*
# Fuego does not understand basic ko...
