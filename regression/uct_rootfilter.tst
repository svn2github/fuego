#-----------------------------------------------------------------------------
# Tests for GoUctDefaultRootFilter
#-----------------------------------------------------------------------------

loadsgf sgf/games/2008/CGOS/540532.sgf 41

# Test that ladder part of root filter does not filter G9
10 uct_root_filter
#? []

loadsgf sgf/games/2008/KGS/7/27/hahaha6k-Fuego9.sgf

# Needs to always generate capturing moves in own safe territory, even if
# current rules do no use CaptureDead(), because the UCT player always scores
# with Tromp-Taylor after two passes in the in-tree phase
20 uct_root_filter
#? [F1 G1 J1 A7 G7 G8 H9 J9]

loadsgf sgf/games/2012/2012-11-25-zen-vs-pro/1.continuation.sgf 35

30 uct_root_filter
#? []*
# G7 must not be filtered here.

loadsgf sgf/games/2012/2012-11-25-zen-vs-pro/1.continuation.sgf 37

40 uct_root_filter
#? []*
# H7 should not be filtered here.
