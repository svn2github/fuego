boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 6

reg_genmove b

10 uct_stat_search count
#? [10000]

20 uct_stat_search games_played
#? [10000]

30 uct_stat_search nodes
#? [753179]

40 uct_sequence
#? [VAR B E6 W D3 B E3 W E4 B F3 W F4 B G3 W G4 B H4 W H5 B J4 W H3 B H2 W J2 B H1 W G2]

50 uct_value
#? [0.494355]
