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
#? [678384]

40 uct_sequence
#? [VAR B C7 W F7 B G6 W H7 B G7 W G8 B H8 W H9 B F8 W G9 B E7 W E8]

50 uct_value
#? [0.498316]
