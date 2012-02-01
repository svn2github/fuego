boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 8

reg_genmove b

10 uct_stat_search count
#? [10000]

20 uct_stat_search games_played
#? [10000]

30 uct_stat_search nodes
#? [757074]

40 uct_sequence
#? [VAR B G3 W C3 B D3 W D2 B E3 W C2 B E2 W E1 B F1 W G1 B F2 W F3]

50 uct_value
#? [0.500111]
