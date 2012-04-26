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
#? [756108]

40 uct_sequence
#? [VAR B G3 W C6 B C4 W C3 B B4 W B3 B A3 W A2 B A4 W B2 B PASS]

50 uct_value
#? [0.508237]
