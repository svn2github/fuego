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
#? [C2]

241 reg_genmove w
#? [C2]

loadsgf sgf/lifeanddeath/nakade-5pts-4.sgf 1

250 reg_genmove w
#? [C2]

260 reg_genmove b
#? [D1|C2]
# D1 is better

loadsgf sgf/lifeanddeath/nakade-5pts-5.sgf 1

270 reg_genmove w
#? [C2]

280 reg_genmove b
#? [C2]
# D1 is better

loadsgf sgf/lifeanddeath/nakade-5pts-6.sgf 1

290 reg_genmove b
#? [B2]*

300 reg_genmove w
#? [B2]

loadsgf sgf/lifeanddeath/nakade-5pts-6b.sgf 2

310 reg_genmove b
#? [B1|B2]

320 reg_genmove w
#? [B2]*
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

411 reg_genmove w
#? [B2]*
# This is difficult for Fuego as of revision 1923,
# it cannot see that the Black
# corner is pretty safe even after killing W

loadsgf sgf/lifeanddeath/nakade-6pts-4b.sgf

420 reg_genmove b
#? [B2]

421 reg_genmove w
#? [B2]*
#easier version where B corner is more stable. Fuego 1923 still has trouble.

loadsgf sgf/lifeanddeath/nakade-corner-1.sgf 1

430 reg_genmove b
#? [G2]
# Test if it can play 6 stone selfatari - only way to win

loadsgf sgf/lifeanddeath/nakade-corner-1.sgf 3

440 reg_genmove b
#? [H1|H2]
# H2 is the "clean" solution. H1 is either ko or it reverts, 
# anyway it also wins here.

450 reg_genmove w
#? [H1|H2]

loadsgf sgf/lifeanddeath/nakade-corner-1.sgf 5

460 reg_genmove b
#? [H1|G1]

470 reg_genmove w
#? [H1]

loadsgf sgf/lifeanddeath/nakade-corner-1.sgf 7

480 reg_genmove b
#? [H1]

490 reg_genmove w
#? [C1|E5|F5|G5|H5|J5]*

loadsgf sgf/lifeanddeath/nakade-corner-1.sgf 9

500 reg_genmove b
#? [J1|G2]

510 reg_genmove w
#? [C1|F5|G5|H5|J5]

loadsgf sgf/lifeanddeath/nakade-corner-1.sgf 11

520 reg_genmove b
#? [H1]

530 reg_genmove w
#? [H1]

loadsgf sgf/lifeanddeath/nakade-corner-1.sgf 13

540 reg_genmove b
#? [H2|G1|J1]

550 reg_genmove w
#? [C1|F5|H5|J5]

loadsgf sgf/lifeanddeath/nakade-corner-1.sgf 15

560 reg_genmove b
#? [H2|J1]

570 reg_genmove w
#? [C1|F5|J5]
