#-----------------------------------------------------------------------------
# Life and Death tests involving nakade shapes.
#-----------------------------------------------------------------------------

loadsgf sgf/games/2011/KGS/FuegoBot2-laska0-3.sgf 40
10 reg_genmove w
#? [D2|H7]

11 reg_genmove w
#? [!G1]

12 reg_genmove w
#? [!C1]
# make seki, or just leave it. G1 and C1 are self-kill.

loadsgf sgf/lifeanddeath/nakade-6pts-realgame.sgf 2
20 reg_genmove w
#? [N11]

loadsgf sgf/lifeanddeath/nakade-6pts-realgame.sgf 4
30 reg_genmove w
#? [M13]

loadsgf sgf/lifeanddeath/nakade-6pts-realgame.2.sgf 4
40 reg_genmove w
#? [L13]

loadsgf sgf/lifeanddeath/nakade-6pts-realgame.2.sgf 6
50 reg_genmove w
#? [N13]
