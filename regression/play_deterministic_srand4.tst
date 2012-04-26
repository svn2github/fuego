boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 4

reg_genmove b

10 uct_stat_search count
#? [10000]

20 uct_stat_search games_played
#? [10000]

30 uct_stat_search nodes
#? [755583]

40 uct_sequence
#? [VAR B F3 W C3 B D4 W E2 B G2 W PASS]

50 uct_value
#? [0.49343]
