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
#? [753894]

40 uct_sequence
#? [VAR B E6 W F4 B E4 W E3 B F3 W F2 B E2 W D3 B D2 W C3]

50 uct_value
#? [0.49784]
