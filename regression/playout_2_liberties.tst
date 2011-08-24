#-----------------------------------------------------------------------------
# Test two liberty policies.
#-----------------------------------------------------------------------------

uct_param_player search_mode playout_policy

#-----------------------------------------------------------------------------

loadsgf sgf/playout/playout_two_liberties_throw_in.sgf 2
10 reg_genmove w
#? [J3]*
# must throw in to win semeai.

11 uct_policy_moves
#? [LowLib J3]*

loadsgf sgf/playout/playout_two_liberties_throw_in.sgf 4
20 reg_genmove w
#? [J3]*
# must throw in to win semeai.

21 uct_policy_moves
#? [LowLib J3]*

loadsgf sgf/playout/playout_two_liberties_throw_in.sgf 6
30 reg_genmove w
#? [J3]
# capture.

31 uct_policy_moves
#? [AtariCapture J3]

loadsgf sgf/playout/playout_two_liberties_semeai.1.sgf 2
40 reg_genmove w
#? [A9|C9]*

41 uct_policy_moves
#? [LowLib A9 C9]*

loadsgf sgf/playout/playout_two_liberties_semeai.1.sgf 4
50 reg_genmove w
#? [B9|C9]*

51 uct_policy_moves
#? [LowLib B9 C9]*

loadsgf sgf/playout/playout_two_liberties_semeai.1.sgf 6
60 reg_genmove w
#? [B9]

61 uct_policy_moves
#? [AtariDefend B9]

loadsgf sgf/playout/playout_two_liberties_semeai.2.sgf 2
70 reg_genmove b
#? [J8]*

71 uct_policy_moves
#? [LowLib J8]*

loadsgf sgf/playout/playout_two_liberties_semeai.2.sgf 4
80 reg_genmove b
#? [J9]

81 uct_policy_moves
#? [AtariDefend J9]

loadsgf sgf/playout/playout_two_liberties_semeai.2.sgf 6
90 reg_genmove b
#? [H7]*

91 uct_policy_moves
#? [Nakade H7]*
# @todo: No nakade rules yet.

loadsgf sgf/playout/playout_two_liberties_semeai.3.sgf 2
100 reg_genmove w
#? [H7|J7]*

101 uct_policy_moves
#? [LowLib H7 J7]*

loadsgf sgf/playout/playout_two_liberties_semeai.4.sgf 2
110 reg_genmove w
#? [A2]
# A1 is also OK in this case but A2 is natural.

111 uct_policy_moves
#? [LowLib A2]*

loadsgf sgf/playout/playout_two_liberties_semeai.5.sgf 2
120 reg_genmove w
#? [H5|J4]*
# Win without a ko.

121 uct_policy_moves
#? [LowLib H5 J4]*
# Current policy plays global capture at J1...

