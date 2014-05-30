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
#? [G2]

loadsgf sgf/lifeanddeath/nakade-6pts-corner-1.sgf 4

#100 reg_genmove w
# #? [F1]
# removed since W is lost, and Fuego wants to resign here

loadsgf sgf/lifeanddeath/nakade-6pts-corner-1.sgf 5

110 reg_genmove b
#? [H2]

loadsgf sgf/lifeanddeath/nakade-6pts-corner-1.sgf 7

120 reg_genmove b
#? [J2]

loadsgf sgf/lifeanddeath/nakade-4pts.1.sgf

130 reg_genmove b
#? [B1]

140 reg_genmove w
#? [B1]

loadsgf sgf/lifeanddeath/nakade-4pts.2.sgf

150 reg_genmove b
#? [C1]

160 reg_genmove w
#? [C1]

loadsgf sgf/lifeanddeath/nakade-4pts.3.sgf

170 reg_genmove b
#? [C1]

180 reg_genmove w
#? [C1]

loadsgf sgf/lifeanddeath/nakade-4pts.4.sgf

190 reg_genmove b
#? [B1]

200 reg_genmove w
#? [B1]

loadsgf sgf/lifeanddeath/nakade-4pts.5.sgf

210 reg_genmove b
#? [C1]

220 reg_genmove w
#? [C1|D1]
# of course D1 is bigger but both lead to a very safe win

loadsgf sgf/lifeanddeath/nakade-4pts.6.sgf 1

230 reg_genmove b
#? [D2|C2|C1]

231 reg_genmove w
#? [C1]

loadsgf sgf/lifeanddeath/nakade-4pts.6.sgf 3

240 reg_genmove b
#? [C1]

241 reg_genmove w
#? [C1]

loadsgf sgf/lifeanddeath/nakade-5pts-4.sgf

250 reg_genmove w
#? [C2]

260 reg_genmove b
#? [D1|C2]
# D1 is better

loadsgf sgf/lifeanddeath/nakade-5pts-5.sgf

270 reg_genmove w
#? [C2]

280 reg_genmove b
#? [D1|C2]
# D1 is better

loadsgf sgf/lifeanddeath/nakade-5pts-6.sgf

290 reg_genmove b
#? [B2]

300 reg_genmove w
#? [B2]

loadsgf sgf/lifeanddeath/nakade-5pts-6b.sgf 2

310 reg_genmove b
#? [B1|B2]

320 reg_genmove w
#? [B2]
# ko

loadsgf sgf/lifeanddeath/nakade-5pts-7.sgf

330 reg_genmove b
#? [C2]

340 reg_genmove w
#? [C2]

loadsgf sgf/lifeanddeath/nakade-5pts-8.sgf

350 reg_genmove b
#? [C2]

360 reg_genmove w
#? [C2]

loadsgf sgf/lifeanddeath/nakade-5pts-9.sgf

370 reg_genmove b
#? [C2]

380 reg_genmove w
#? [C2]

loadsgf sgf/lifeanddeath/nakade-5pts-10.sgf

390 reg_genmove b
#? [C2]

400 reg_genmove w
#? [C2]

loadsgf sgf/lifeanddeath/nakade-6pts-4.sgf

410 reg_genmove b
#? [B2]

420 reg_genmove w
#? [B2]

loadsgf sgf/lifeanddeath/nakade-corner-1.sgf 3

430 reg_genmove b
#? [G2]
# not best test case, since already dead. But want to test if it can play 
# 6 stone selfatari

loadsgf sgf/lifeanddeath/nakade-corner-1.sgf 5

440 reg_genmove b
#? [H2]

450 reg_genmove w
#? [H1|H2]

loadsgf sgf/lifeanddeath/nakade-corner-1.sgf 7

460 reg_genmove b
#? [J2]

470 reg_genmove w
#? [J2|G2]
