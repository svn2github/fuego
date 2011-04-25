#-----------------------------------------------------------------------------
# Tests involving seki for static safety solver
# @todo Currently, there is no static seki recognition, so just
# recognizes the territories
#-----------------------------------------------------------------------------

loadsgf sgf/seki/seki-eye-vs-eye.1.sgf

10 go_safe static black
#? [0]
# full answer with seki detection: 16

11 go_safe static white
#? [16 .*]
# full answer with seki detection: 19

12 go_safe_dame_static
#? []

# 13 go_safe_shared_liberties_in_seki
# B5

loadsgf sgf/seki/seki-eye-vs-no-eye.1.sgf

20 go_safe static black
#? [0]
# full answer with seki detection: 12

21 go_safe static white
#? [16 .*]
# full answer with seki detection: 22

22 go_safe_dame_static
#? []

# 23 go_safe_shared_liberties_in_seki
# B5 E2

loadsgf sgf/seki/static-safety-seki-bug.sgf

30 go_safe benson black
#? [18 .*]

31 go_safe benson white
#? [16 .*]

40 go_safe static black
#? [48]*
# misclassifies seki as safe black
# 25 pts for top left, 5 pts for seki stones

41 go_safe static white
#? [30 .*]*

42 go_safe_dame_static
#? [E6]*

# 43 go_safe_shared_liberties_in_seki
# F9 J7

loadsgf sgf/seki/safety-solver-seki-problem.sgf 41

50 go_safe static black
#? [0]
# B 4 stones are at-most-seki, so cannot count them

51 go_safe static white
#? [13 .*]*
# W group is at-least-seki

loadsgf sgf/seki/safety-solver-seki-problem.sgf 42
60 go_safe static black
#? [0]
# B 5 stones are at-most-seki, so cannot count them

61 go_safe static white
#? [13 .*]*
# W group is at-least-seki

loadsgf sgf/seki/safety-solver-seki-problem.sgf 56
70 go_safe static black
#? [32 .*]*
# B 5 stones are seki now

71 go_safe static white
#? [15 .*]*
# W group is seki

loadsgf sgf/lifeanddeath/nakade-5pts-1.sgf
80 go_safe static black
#? [65 .*]

81 go_safe static white
#? [16 .*]

loadsgf sgf/lifeanddeath/nakade-5pts-2.sgf
90 go_safe static black
#? [65 .*]

91 go_safe static white
#? [16 .*]

loadsgf sgf/lifeanddeath/nakade-5plus-seki.sgf
# cannot solve this seki statically without database?

100 go_safe static black
#? [42 .*]

101 go_safe static white
#? [16 .*]

loadsgf sgf/lifeanddeath/nakade-6pts-1.sgf

110 go_safe static black
#? [65 .*]*

111 go_safe static white
#? [16 .*]

loadsgf sgf/lifeanddeath/nakade-6pts-2.sgf

120 go_safe static black
#? [65 .*]

121 go_safe static white
#? [16 .*]

loadsgf sgf/lifeanddeath/nakade-6pts-3.sgf

130 go_safe static black
#? [65 .*]*

131 go_safe static white
#? [16 .*]

loadsgf sgf/lifeanddeath/no-nakade-6pts-1.sgf
# W is safe, but not statically

140 go_safe static black
#? [42 .*]

141 go_safe static white
#? [16 .*]


