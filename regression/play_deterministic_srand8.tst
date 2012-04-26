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
#? [679359]

40 uct_sequence
#? [VAR B F7 W C5 B D5 W D4 B E5 W C4 B E4 W E3 B D3 W D2 B E2]

50 uct_value
#? [0.505254]
