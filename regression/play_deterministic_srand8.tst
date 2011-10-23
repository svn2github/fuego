boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 8

reg_genmove b

10 uct_stat_search count
#? [9000]

20 uct_stat_search games_played
#? [9000]

30 uct_stat_search nodes
#? [677338]

40 uct_sequence
#? [VAR B D6 W D3 B G7 W G4 B G3 W F3 B H3 W H4 B J4 W J5 B J3]

50 uct_value
#? [0.504613]
