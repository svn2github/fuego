boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 7

reg_genmove b

10 uct_stat_search count
#? [10000]

20 uct_stat_search games_played
#? [10000]

30 uct_stat_search nodes
#? [757479]

40 uct_sequence
#? [VAR B F3 W C3 B E3 W D5 B C4 W B4 B D4 W B5 B C5 W D6 B C6 W B6]

50 uct_value
#? [0.510699]
