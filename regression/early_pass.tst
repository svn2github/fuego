#-----------------------------------------------------------------------------
# Tests related to behavior at end of game if early pass is on.
#-----------------------------------------------------------------------------

uct_param_player early_pass 1

loadsgf sgf/games/2013/Gnugo-Fuego.sgf 276
10 reg_genmove w
#? [N17]*
# a bug in early pass