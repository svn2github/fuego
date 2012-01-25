#-----------------------------------------------------------------------------
# Position evaluation tests involving two safe white groups on the 9x9 board.
# All these positions are lost for Black so the uct_value should be low.
# The positions in this file are closely related to two_safe_groups*.tst.
# However, here one of the two groups is "played out" so it should always
# be safe even in the playouts. This allows a global search to focus one 
# the other group which is not so solid.
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------

loadsgf sgf/two-safe-groups/case1-1.sgf
reg_genmove b
10 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case1-2.sgf
reg_genmove b
15 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case2-1.sgf
reg_genmove b
20 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case2-2.sgf
reg_genmove b
20 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case3-1.sgf
reg_genmove b
30 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case3-2.sgf
reg_genmove b
35 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case4-1.sgf
reg_genmove b
40 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case4-2.sgf
reg_genmove b
45 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case5-1.sgf
reg_genmove b
50 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case5-2.sgf
reg_genmove b
55 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case6-1.sgf
reg_genmove b
60 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case6-2.sgf
reg_genmove b
65 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case7-1.sgf
reg_genmove b
70 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case7-2.sgf
reg_genmove b
75 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case8-1.sgf
reg_genmove b
80 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case8-2.sgf
reg_genmove b
85 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case9-1.sgf
reg_genmove b
90 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case9-2.sgf
reg_genmove b
95 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case10-1.sgf
reg_genmove b
100 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case10-2.sgf
reg_genmove b
105 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case11-1.sgf
reg_genmove b
110 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case11-2.sgf
reg_genmove b
115 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case12-1.sgf
reg_genmove b
120 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case12-2.sgf
reg_genmove b
125 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case13-1.sgf
reg_genmove b
130 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case13-2.sgf
reg_genmove b
135 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case14-1.sgf
reg_genmove b
140 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case14-2.sgf
reg_genmove b
145 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case15-1.sgf
reg_genmove b
150 sg_compare_float 0.3 uct_value
#? [-1]

loadsgf sgf/two-safe-groups/case15-2.sgf
reg_genmove b
155 sg_compare_float 0.3 uct_value
#? [-1]