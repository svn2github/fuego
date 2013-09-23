#-----------------------------------------------------------------------------
# scoring_final: end-of-game scoring using the Fuego engine.
# Chinese rules.
# Test cases mostly from games where other programs such as GNU Go and pachi
# scored differently.
#-----------------------------------------------------------------------------

loadsgf sgf/scoring/FX178-27.sgf

10 final_score
#? [B\+35\.5]

loadsgf sgf/scoring/FX175-29-variation.sgf

20 final_score
#? [B\+31\.5]*
# Fuego does not recognize bottom right as dead.
 
