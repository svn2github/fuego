#-----------------------------------------------------------------------------
# Tests for GoUctDefaultRootFilter
#-----------------------------------------------------------------------------

loadsgf sgf/games/2008/CGOS/540532.sgf 41

showboard

# Test that ladder part of root filter does not filter G9
10 uct_root_filter
#? []
