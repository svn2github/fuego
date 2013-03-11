#-----------------------------------------------------------------------------
# Long ladder blunders from Fuego 19x19 games. 
# Capture or escape blunders that could be avoided by reading a ladder.
#-----------------------------------------------------------------------------

loadsgf sgf/games/2008/CGOS/31425.sgf 90
10 reg_genmove white
#? [!L12]

loadsgf sgf/games/2008/CGOS/31425.sgf 91
15 reg_genmove black
#? [M13]

loadsgf sgf/games/2008/CGOS/31425.sgf 92
20 reg_genmove white
#? [!M14]

loadsgf sgf/games/2008/CGOS/31425.sgf 93
25 reg_genmove black
#? [L13]

loadsgf sgf/games/2008/CGOS/31425.sgf 94
30 reg_genmove white
#? [!K13]*

loadsgf sgf/games/2008/CGOS/31425.sgf 95
35 reg_genmove black
#? [L14]

loadsgf sgf/games/2008/CGOS/31425.sgf 98
36 reg_genmove white
#? [!L15]*

loadsgf sgf/games/2008/CGOS/31425.sgf 99
37 reg_genmove black
#? [K14]

loadsgf sgf/ladder/ladder-blunder.sgf 32
40 reg_genmove white
#? [!(G6|H6)]*
# testing only a few moves, not all the bad ladder

loadsgf sgf/ladder/ladder-blunder.sgf 33
50 reg_genmove black
#? [H6]

loadsgf sgf/ladder/ladder-blunder.sgf 34
60 reg_genmove white
#? [!H7]*
# testing only a few moves, not all the bad ladder

loadsgf sgf/ladder/ladder-blunder.sgf 42
70 reg_genmove white
#? [!K9]

loadsgf sgf/ladder/ladder-blunder.sgf 43
75 reg_genmove black
#? [L8]

loadsgf sgf/ladder/ladder-blunder.sgf 52
80 reg_genmove white
#? [!O10]

loadsgf sgf/ladder/ladder-blunder.sgf 53
90 reg_genmove black
#? [N11]

loadsgf sgf/ladder/bad-ladder-fuego-erica-1.sgf 78
100 reg_genmove white
#? [!L15]

101 reg_genmove white
#? [K14|F17]

loadsgf sgf/ladder/bad-ladder-fuego-erica-1.sgf 79
110 reg_genmove black
#? [K14]

# for move 80, chasing the bad ladder may be the best thing to do?
# so no test for that.

loadsgf sgf/ladder/bad-ladder-fuego-erica-1.sgf 82
120 reg_genmove white
#? [!M14]*

loadsgf sgf/ladder/bad-ladder-fuego-erica-1.sgf 86
130 reg_genmove white
#? [!N13]*
# unstable between M12, F17 and N13

loadsgf sgf/ladder/bad-ladder-fuego-erica-1.sgf 90
140 reg_genmove white
#? [!O12]
# unstable, plays each atari about half the time?

loadsgf sgf/ladder/fuego-erica-2-ladder-variation.sgf 27
150 reg_genmove black
#? [D8]

loadsgf sgf/ladder/fuego-erica-2-ladder-variation.sgf 28
160 reg_genmove white
#? [!(D9|D7)]*
# D9 is still a ladder for Black

loadsgf sgf/ladder/fuego-erica-2-ladder-variation.sgf 29
170 reg_genmove black
#? [D10]

loadsgf sgf/ladder/fuego-erica-2-ladder-variation.sgf 30
180 reg_genmove white
#? [!D7]*
#  still a ladder for Black

loadsgf sgf/ladder/fuego-erica-2-ladder-variation.sgf 31
190 reg_genmove black
#? [E7]*
# unstable. Sometimes extends on E8 instead.

# 32 capture is probably OK, no test.

loadsgf sgf/ladder/fuego-erica-2-ladder-variation.sgf 33
200 reg_genmove black
#? [D6]

loadsgf sgf/ladder/fuego-erica-2-ladder-variation.sgf 34
210 reg_genmove white
#? [!D8]

loadsgf sgf/ladder/fuego-erica-2-ladder-variation.sgf 35
220 reg_genmove black
#? [F8]

loadsgf sgf/ladder/fuego-erica-2-ladder-variation.sgf 36
230 reg_genmove white
#? [!E9]




