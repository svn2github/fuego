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
#? [757550]

40 uct_sequence
#? [VAR B G5 W C5 B D6 W C6 B D7 W D5 B C7 W B7 B B6 W A6 B A5 W A7 B B8 W B5]

50 uct_value
#? [0.490284]
