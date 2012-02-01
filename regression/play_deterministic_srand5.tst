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
#? [757767]

40 uct_sequence
#? [VAR B C6 W D4 B B5 W D6 B D7 W D5 B C7 W C5 B B4 W B6 B A6 W B7]

50 uct_value
#? [0.508596]
