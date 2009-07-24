#-----------------------------------------------------------------------------
# Test the opening book using the book_moves command
#-----------------------------------------------------------------------------

book_load ../book/book.dat

#-----------------------------------------------------------------------------
loadsgf sgf/games/2007/CGOS/200368.sgf 9
10 book_moves
#? [D2]
# E2 seems OK too.

loadsgf sgf/games/2007/CGOS/193991.sgf 2
20 book_moves
#? [E4]

loadsgf sgf/games/2008/CGOS/595283.sgf 5
30 book_moves
#? [E3]
# book move was C4, but this seems refuted by this game.
# See followup tests below.
# then it was D3.
# Now back to E3.

loadsgf sgf/games/2008/CGOS/595283.sgf 6
40 book_moves
#? [D4]

loadsgf sgf/games/2008/CGOS/595283.sgf 8
50 book_moves
#? [C3]

loadsgf sgf/games/2008/CGOS/595283.sgf 10
60 book_moves
#? [C6]

loadsgf sgf/games/2008/CGOS/595283.sgf 12
70 book_moves
#? [E4]

loadsgf sgf/games/2008/CGOS/595283.sgf 14
80 book_moves
#? [E3]

loadsgf sgf/games/2008/CGOS/595283.sgf 16
90 book_moves
#? [F5]

loadsgf sgf/games/2008/CGOS/595283.sgf 18
100 book_moves
#? [G6]

loadsgf sgf/games/2008/CGOS/595042.sgf 10
110 book_moves
#? [C4]
# This whole line is a big headache. This game seems to refute D8.
# The bottom group is too weak after G3.
# not convinced that C4 is enough, either. Maybe W needs to deviate earlier.
