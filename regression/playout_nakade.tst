#-----------------------------------------------------------------------------
# Test playout policy on nakade shapes.
#-----------------------------------------------------------------------------

uct_param_player search_mode playout_policy

#-----------------------------------------------------------------------------

loadsgf sgf/playout/playout_nakade.1.sgf 3
10 reg_genmove w
#? [J8]
# must make nakade shape.

11 uct_policy_moves
#? [AtariCapture J8]
# todo: AtariCapture is kind of a silly reason

loadsgf sgf/playout/playout_nakade.1.sgf 5
20 reg_genmove w
#? [H8]*
# must kill on vital point.

21 uct_policy_moves
#? [Nakade H8]*
# Nakade policy does not exist yet.

loadsgf sgf/playout/playout_nakade.1.sgf 7
30 reg_genmove w
#? [J8]
# must kill on vital point.

31 uct_policy_moves_simple
#? [GammaPattern J8]
# Should be different move reason in future ???

loadsgf sgf/playout/playout_nakade.2.sgf
40 uct_policy_moves
#? [GammaPattern E1:1 F1:550.309]
# F1 is much more likely, but E1 would be fatal...

41 uct_policy_moves_simple
#? [GammaPattern F1]*

loadsgf sgf/playout/playout_nakade.3.sgf
50 uct_policy_moves_simple
#? [GammaPattern F2]*
# Feature knowledge has a high prior for f2, but gamma patterns don't.

loadsgf sgf/playout/playout_nakade.4.sgf 1
60 uct_policy_moves_simple
#? [GammaPattern F1]*
# should know to play in center of 3 point nakade

loadsgf sgf/nakade/nakade.1.sgf
70 uct_policy_moves_simple
#? [GammaPattern F8|H8]*
# plays F9 or H9 now which is fatal

loadsgf sgf/nakade/nakade.2.sgf
80 uct_policy_moves_simple
#? [Nakade B1 C1]*
# random move now.

81 uct_policy_corrected_moves
#? [FillNakadeCorrection A1 B1 FillNakadeCorrection D1 C1]*
# should move a1 to b1 and d1 to c1 for White as well.

loadsgf sgf/nakade/nakade.2.sgf 1
85 uct_policy_corrected_moves
#? [SelfAtariCorrection A1 B1 SelfAtariCorrection D1 C1]

loadsgf sgf/nakade/nakade.3.sgf
90 uct_policy_moves_simple
#? [Nakade B1]*
# Make 3 point nakade. random move now.

95 uct_policy_corrected_moves
#? [SelfAtariCorrection A1 B1 SelfAtariCorrection C1 B1]

loadsgf sgf/nakade/nakade.3b.sgf
100 uct_policy_moves_simple
#? [Nakade B1]*
# Same as nakade.3 for defender's color - make eyes. random move now. 

105 uct_policy_corrected_moves
#? [FillNakadeCorrection A1 B1 FillNakadeCorrection C1 B1]*

loadsgf sgf/nakade/nakade.4.sgf
110 uct_policy_moves_simple
#? [GammaPattern C1]

loadsgf sgf/nakade/nakade.4b.sgf
120 uct_policy_moves_simple
#? [Nakade C1]*
# a good selfatari

125 uct_policy_corrected_moves
#? [SelfAtariCorrection D1 C1]*
# 2 stones selfatari here is much better than two single selfatari stones
# since it creates nakade

loadsgf sgf/nakade/nakade.4c.sgf
130 uct_policy_moves_simple
#? [Nakade C1]*

135 uct_policy_corrected_moves
#? [SelfAtariCorrection A1 C1 SelfAtariCorrection D1 C1]*
# missing D1 C1, bug.

loadsgf sgf/nakade/nakade.5.sgf
140 uct_policy_moves_simple
#? [GammaPattern A6]*
# This may be too hard to encode in playout policy. 
# Do not play in nakade but make eye elsewhere.
# At least check we do not make the nakade smaller?
# In general should there be a bonus for eye-making and -taking moves?
# For small groups only?

loadsgf sgf/nakade/nakade.6.sgf
150 uct_policy_moves_simple
#? [Nakade G8]*

loadsgf sgf/nakade/nakade.6b.sgf
160 uct_policy_moves_simple
#? [Nakade G8]*

loadsgf sgf/nakade/nakade.6c.sgf
170 uct_policy_moves_simple
#? [Nakade G8]*
# same position as nakade.6 but last move info is available

loadsgf sgf/nakade/nakade.6d.sgf
180 uct_policy_moves_simple
#? [Nakade G8]*
# same position as nakade.6b but last move info is available

loadsgf sgf/nakade/nakade.7.sgf
190 uct_policy_moves_simple
#? [Nakade H9]*

loadsgf sgf/nakade/nakade.8.sgf
200 uct_policy_moves_simple
#? [Nakade H9]*

loadsgf sgf/nakade/nakade.9.sgf
210 uct_policy_moves_simple
#? [GammaPattern G8]*
# Plays H9 blunder now.

