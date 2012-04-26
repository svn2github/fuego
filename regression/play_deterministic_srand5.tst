boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 5

reg_genmove b

10 uct_stat_search count
#? [10000]

20 uct_stat_search games_played
#? [10000]

30 uct_stat_search nodes
#? [754304]

40 uct_sequence
#? [VAR B G7 W F3 B F5 W F4 B E4 W E5 B E6 W D5 B D4 W C5]

50 uct_value
#? [0.501778]
