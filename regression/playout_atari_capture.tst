#-----------------------------------------------------------------------------
# Tests using playout policy directly for move generation.
#-----------------------------------------------------------------------------

uct_param_player search_mode playout_policy

#-----------------------------------------------------------------------------
# Global Capture heuristic

loadsgf sgf/playout/playout_capture_1.sgf
10 reg_genmove b
#? [F3]

11 uct_policy_moves
#? [Capture F3]


#-----------------------------------------------------------------------------
# Atari defense heuristic

loadsgf sgf/playout/playout_atari_defense_1.sgf
110 reg_genmove b
#? [B1]

loadsgf sgf/playout/playout_atari_defense_2.sgf
120 reg_genmove w
#? [B3]

loadsgf sgf/playout/playout_atari_defense_3.sgf
130 reg_genmove w
#? [D2]

loadsgf sgf/playout/playout_two_liberties_semeai.4.sgf 4
140 reg_genmove w
#? [A1]

141 uct_policy_moves
#? [AtariDefend A1]

#-----------------------------------------------------------------------------
# Atari capture heuristic

loadsgf sgf/playout/playout_two_liberties_semeai.3.sgf 4
200 reg_genmove w
#? [J7]*

201 uct_policy_moves
#? [AtariCapture J7]*
# current policy captures F4...
