boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 9

reg_genmove b

10 uct_stat_search count
#? [10000]

20 uct_stat_search games_played
#? [10000]

30 uct_stat_search nodes
#? [756619]

40 uct_sequence
#? [VAR B G4 W H5 B G5 W H4 B H6 W G3 B F3 W H3 B G2 W F2 B F1]

50 uct_value
#? [0.498414]
