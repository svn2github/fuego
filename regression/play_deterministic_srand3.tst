boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 3

reg_genmove b

10 uct_stat_search count
#? [9000]

20 uct_stat_search games_played
#? [9000]

30 uct_stat_search nodes
#? [677964]

40 uct_sequence
#? [VAR B C3 W G6 B E5 W G5 B F3 W H4 B PASS]

50 uct_value
#? [0.491714]
