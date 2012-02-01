boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 2

reg_genmove b

10 uct_stat_search count
#? [9000]

20 uct_stat_search games_played
#? [9000]

30 uct_stat_search nodes
#? [678580]

40 uct_sequence
#? [VAR B G6 W C7 B G3 W B5 B B6 W C5 B A5 W A4 B A6 W B4 B PASS]

50 uct_value
#? [0.506187]
