#-----------------------------------------------------------------------------
# Position evaluation tests for Fuego.
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------

loadsgf sgf/evaluation/miseval.1.sgf
reg_genmove w
10 sg_compare_float 0.5 uct_value
#? [1]*
# takes a long time to see W can simply capture and then win the ko.