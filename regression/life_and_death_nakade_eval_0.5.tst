#-----------------------------------------------------------------------------
# Full board evaluation for Life and Death tests involving nakade shapes.
#-----------------------------------------------------------------------------

uct_param_player early_pass 0
# must turn off early pass, otherwise the uct_value could be 
# from the pass search, not the normal search.

loadsgf sgf/lifeanddeath/nakade-3pts-1.sgf
reg_genmove b
10 sg_compare_float 0.5 uct_value
#? [1]
# W is dead, B wins

reg_genmove w
11 sg_compare_float 0.5 uct_value
#? [-1]

# 12 sg_compare_float 0.8 point_score F3
##? [1]
# F3 should be Black

# @todo verify that Black can play the cleanup/capture moves correctly

loadsgf sgf/lifeanddeath/nakade-5pts-3.sgf
reg_genmove b
20 sg_compare_float 0.5 uct_value
#? [1]*
# W is dead, B wins

reg_genmove w
21 sg_compare_float 0.5 uct_value
#? [-1]*

# 22 sg_compare_float 0.8 point_score F3
##? [1]*
# F3 should be Black

# @todo verify that Black can play the cleanup/capture moves correctly

loadsgf sgf/lifeanddeath/seki-nakade-3pts.sgf
reg_genmove b
30 sg_compare_float 0.45 uct_value
#? [1]

31 sg_compare_float 0.55 uct_value
#? [-1]
# jigo (tie), score 0.50

reg_genmove w
32 sg_compare_float 0.45 uct_value
#? [1]

33 sg_compare_float 0.55 uct_value
#? [-1]

# @todo both players should pass here but still play inside territories
# @todo e.g. check under Japanese rules
# @todo check point_score for all stones, empty points in seki

loadsgf sgf/lifeanddeath/seki-nakade-3pts.2.sgf

reg_genmove b
40 sg_compare_float 0.45 uct_value
#? [1]

41 sg_compare_float 0.55 uct_value
#? [-1]
# jigo (tie), score 0.50

reg_genmove w
42 sg_compare_float 0.45 uct_value
#? [1]

43 sg_compare_float 0.55 uct_value
#? [-1]

# @todo both players should pass here but still play inside territories
# @todo e.g. check under Japanese rules
# @todo check point_score for all stones, empty points in seki

loadsgf sgf/lifeanddeath/seki-nakade-5pts.sgf

reg_genmove b
50 sg_compare_float 0.45 uct_value
#? [1]

51 sg_compare_float 0.55 uct_value
#? [-1]
# jigo (tie), score 0.50

reg_genmove w
52 sg_compare_float 0.45 uct_value
#? [1]

53 sg_compare_float 0.55 uct_value
#? [-1]

# @todo both players should pass here, or B can play G1
# currently Fuego still plays inside territories
# @todo e.g. check under Japanese rules
# @todo check point_score for all stones, empty points in seki

loadsgf sgf/lifeanddeath/seki-nakade-5pts.2.sgf 1

reg_genmove b
60 sg_compare_float 0.45 uct_value
#? [1]

61 sg_compare_float 0.55 uct_value
#? [-1]
# jigo (tie), score 0.50

reg_genmove w
62 sg_compare_float 0.45 uct_value
#? [1]

63 sg_compare_float 0.55 uct_value
#? [-1]

loadsgf sgf/lifeanddeath/nakade-6pts-corner-1.sgf 1
reg_genmove b
70 sg_compare_float 0.51 uct_value
#? [1]*
# not jigo, W dead
