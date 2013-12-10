#-----------------------------------------------------------------------------
# Bent four in the corner tests.
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------

loadsgf sgf/bent_four/bentfour1.sgf 2
10 reg_genmove b
#? [E5|F5|G5|H5|J5]*
# black can win the fight

loadsgf sgf/bent_four/bentfour1.sgf 4
20 reg_genmove b
#? [G8]

loadsgf sgf/bent_four/bentfour1.sgf 6
30 reg_genmove b
#? [E5|F5|G5|H5]

loadsgf sgf/bent_four/bentfour1.sgf 8
40 reg_genmove b
#? [E5|G5|H5]

loadsgf sgf/bent_four/bentfour1.sgf 10
50 reg_genmove b
#? [J8]

loadsgf sgf/bent_four/bentfour1.sgf 12
60 reg_genmove b
#? [E5|H5]

loadsgf sgf/bent_four/bentfour1.sgf 14
70 reg_genmove b
#? [E5|H5]

loadsgf sgf/bent_four/bentfour1b.sgf 2
80 reg_genmove b
#? [J9]

loadsgf sgf/bent_four/bentfour1b.sgf 4
90 reg_genmove b
#? [G9]

loadsgf sgf/bent_four/bentfour2.sgf 1
100 reg_genmove w
#? [B1|B2]*
# white can win the fight

loadsgf sgf/bent_four/bentfour2.sgf 3
110 reg_genmove w
#? [J8]

loadsgf sgf/bent_four/bentfour2.sgf 5
120 reg_genmove w
#? [H9]

loadsgf sgf/bent_four/bentfour2.sgf 7
130 reg_genmove w
#? [J8]

loadsgf sgf/bent_four/bentfour2.sgf 9
140 reg_genmove w
#? [G9]

loadsgf sgf/bent_four/bentfour2.sgf 11
150 reg_genmove w
#? [A1]

loadsgf sgf/bent_four/bentfour2.sgf 13
160 reg_genmove w
#? [C2]
