boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 4

reg_genmove b

10 uct_stat_search count
#? [10000]

20 uct_stat_search games_played
#? [10000]

30 uct_stat_search nodes
#? [754860]

40 uct_sequence
#? [VAR B D7 W G2 B H7 W D3 B E3 W E4 B F3 W F4 B G4 W G5 B H4 W G3 B E2]

50 uct_value
#? [0.498571]
