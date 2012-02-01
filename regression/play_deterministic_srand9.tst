boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 9

reg_genmove b

10 uct_stat_search count
#? [9000]

20 uct_stat_search games_played
#? [9000]

30 uct_stat_search nodes
#? [678611]

40 uct_sequence
#? [VAR B G6 W C3 B F3 W E8 B B7 W B6 B A6 W A5 B A4 W B4 B B5]

50 uct_value
#? [0.496326]
