#-----------------------------------------------------------------------------
# Tests for static win solver
#-----------------------------------------------------------------------------

loadsgf sgf/static-winner/static-winner.1.sgf 1

10 go_safe_winner
#? [black]
# todo: cases 20,40,60,80 can be solved by taking to-play into account.

loadsgf sgf/static-winner/static-winner.1.sgf 2

20 go_safe_winner
#? [unknown]

loadsgf sgf/static-winner/static-winner.1.sgf 3

30 go_safe_winner
#? [black]

loadsgf sgf/static-winner/static-winner.1.sgf 4

40 go_safe_winner
#? [unknown]

loadsgf sgf/static-winner/static-winner.1.sgf 5

50 go_safe_winner
#? [black]

loadsgf sgf/static-winner/static-winner.1.sgf 6

60 go_safe_winner
#? [unknown]

loadsgf sgf/static-winner/static-winner.1.sgf 7

70 go_safe_winner
#? [black]

loadsgf sgf/static-winner/static-winner.1.sgf 8

80 go_safe_winner
#? [unknown]

loadsgf sgf/static-winner/static-winner.1.sgf 9

90 go_safe_winner
#? [black]



