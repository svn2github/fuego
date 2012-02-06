#-----------------------------------------------------------------------------
# Position evaluation tests involving two safe white groups on the 9x9 board.
# All these positions are lost for Black so the uct_value should be low.
#-----------------------------------------------------------------------------

uct_param_player early_pass 0
# must turn off early pass, otherwise the uct_value could be 
# from the pass search, not the normal search.

#-----------------------------------------------------------------------------

loadsgf sgf/two-safe-groups/case1.sgf
reg_genmove b
10 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case2.sgf
reg_genmove b
20 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case3.sgf
reg_genmove b
30 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case4.sgf
reg_genmove b
40 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case5.sgf
reg_genmove b
50 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case6.sgf
reg_genmove b
60 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case7.sgf
reg_genmove b
70 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case8.sgf
reg_genmove b
80 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case9.sgf
reg_genmove b
90 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case10.sgf
reg_genmove b
100 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case11.sgf
reg_genmove b
110 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case12.sgf
reg_genmove b
120 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case13.sgf
reg_genmove b
130 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case14.sgf
reg_genmove b
140 sg_compare_float 0.5 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case15.sgf
reg_genmove b
150 sg_compare_float 0.5 uct_value
#? [-1]

