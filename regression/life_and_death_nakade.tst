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
#? [N13]*
# unstable, sometimes finds it

loadsgf sgf/lifeanddeath/seki-nakade-5pts.2.sgf 1

60 reg_genmove b
#? [E1|F2|H2]

61 reg_genmove w
#? [PASS]*

62 reg_genmove w
#? [!(E1|F2|H2)]

loadsgf sgf/lifeanddeath/nakade-6pts-corner-1.sgf 1

70 reg_genmove b
#? [G2|J2]

loadsgf sgf/lifeanddeath/nakade-6pts-corner-1.sgf 2

80 reg_genmove w
#? [PASS]*

loadsgf sgf/lifeanddeath/nakade-6pts-corner-1.sgf 3

90 reg_genmove b
#? [G2]*

loadsgf sgf/lifeanddeath/nakade-6pts-corner-1.sgf 4

100 reg_genmove w
#? [F1]

loadsgf sgf/lifeanddeath/nakade-6pts-corner-1.sgf 5

110 reg_genmove b
#? [H2]

loadsgf sgf/lifeanddeath/nakade-6pts-corner-1.sgf 7

120 reg_genmove b
#? [J2]

