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
#? [677184]

40 uct_sequence
#? [VAR B G7 W B4 B C4 W C5 B C3 W B3 B B2 W C2 B C1 W D2 B D3 W D4 B E3 W E2]


50 uct_value
#? [0.499154]
