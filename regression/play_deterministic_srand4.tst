boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 4

reg_genmove b

10 uct_stat_search count
#? [9000]

20 uct_stat_search games_played
#? [9000]

30 uct_stat_search nodes
#? [680619]

40 uct_sequence
#? [VAR B F6 W E3 B C3 W C4 B B4 W C5 B D4 W D3 B E4 W F4 B E5]

50 uct_value
#? [0.49991]
