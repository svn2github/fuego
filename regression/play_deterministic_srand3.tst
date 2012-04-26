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
#? [680012]

40 uct_sequence
#? [VAR B C3 W E3 B E6 W E5 B F5 W E4 B F4 W F3 B G3 W G4 B H4]

50 uct_value
#? [0.504924]
