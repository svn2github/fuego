boardsize 9
book_clear
uct_param_player max_games 10000
deterministic_mode 
set_random_seed 2

reg_genmove b

10 uct_stat_search count
#? [9000]

20 uct_stat_search games_played
#? [9000]

30 uct_stat_search nodes
#? [680554]

40 uct_sequence
#? [VAR B D7 W F5 B G4 W G5 B H5 W H4 B H3 W H6 B J4 W J6 B J5 W J3]

50 uct_value
#? [0.505307]
