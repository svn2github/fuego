boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 5

reg_genmove b

10 uct_stat_search count
#? [9000]

20 uct_stat_search games_played
#? [9000]

30 uct_stat_search nodes
#? [679556]

40 uct_sequence
#? [VAR B C7 W F3 B C4 W G4 B F4 W E4 B G5 W G3 B H4 W H5 B J5 W H6]

50 uct_value
#? [0.489509]
