boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 7

reg_genmove b

10 uct_stat_search count
#? [7500]

20 uct_stat_search games_played
#? [7500]

30 uct_stat_search nodes
#? [563463]

40 uct_sequence
#? [VAR B G6 W F8 B G8 W F7 B G7 W F6 B F5 W G5 B G4 W H5 B H6 W H4 B G3]

50 uct_value
#? [0.496735]
