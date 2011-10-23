boardsize 9

book_clear 

uct_param_player max_games 10000
deterministic_mode 
set_random_seed 3

reg_genmove b

10 uct_stat_search count
#? [10000]

20 uct_stat_search games_played
#? [10000]

30 uct_stat_search nodes
#? [755332]

40 uct_sequence
#? [VAR B G4 W C3 B D3 W D2 B C2 W C1 B B2 W B3 B A2 W B1]

50 uct_value
#? [0.509953]
