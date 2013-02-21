#-----------------------------------------------------------------------------
# cleanup_japanese_rules: tests for finishing the game under Japanese rules.
# Test cases where territory (Japanese) rules demand different play than 
# area (e.g. Chinese) rules.
# Examples: play dame point or pass, do not play in own territory.
# Also see go/GoRules.h
# Also see http://en.wikipedia.org/wiki/Rules_of_Go#Scoring_systems
#-----------------------------------------------------------------------------

go_rules japanese
go_param_rules japanese_scoring 1

# uct_param_player early_pass 0
# Want to switch off early pass since program should play well under Japanese rules
# even without this flag set. But currently that causes many FAIL's.

loadsgf sgf/games/2010/LittleGolem/1173624.sgf 48
10 reg_genmove w
#? [A1]*
# Fuego was winning, but cannot afford to play in its own territory.
# good test case for implementing Japanese rules.

loadsgf sgf/games/2010/LittleGolem/1177582.sgf 39
20 reg_genmove b
#? [PASS|J6|H6|E8|C9]
# Fuego was winning, but cannot afford to play in its own territory.
# Playing a threat in the opponent's area is silly but OK.
# good test case for implementing Japanese rules.

loadsgf sgf/games/2011/UEC/katsunari-Fuego.sgf 299
30 reg_genmove b
#? [K11|L10|L16]
# last neutral points

loadsgf sgf/games/2011/UEC/katsunari-Fuego.sgf 301
40 reg_genmove b
#? [K11|L10]
# last neutral points

loadsgf sgf/games/2011/UEC/katsunari-Fuego.sgf 303
50 reg_genmove b
#? [K11|L10]
# last neutral points

loadsgf sgf/games/2011/UEC/katsunari-Fuego.sgf 325
60 reg_genmove b
#? [K11|L10]
# last neutral points

loadsgf sgf/games/2011/UEC/katsunari-Fuego.sgf 385
70 reg_genmove b
#? [K11|L10]
# last neutral points - last chance to play, still B+0.5

loadsgf sgf/games/2011/UEC/Fuego-LeafQuest.sgf 254
80 reg_genmove w
#? [PASS]
# Fuego feels there are some "unsafe points" so continues to capture dead.

loadsgf sgf/games/2011/UEC/Fuego-LeafQuest.sgf 278
90 reg_genmove w
#? [PASS]
# Bottom right often lives in simulations...
# TODO check territory statistics e.g. on T4 - they are favorable for Black...

loadsgf sgf/games/2011/UEC/Fuego-LeafQuest.sgf 284
100 reg_genmove w
#? [PASS]

loadsgf sgf/games/2011/UEC/shikousakugo-Fuego.sgf 181
110 reg_genmove w
#? [PASS]*
# Safe to pass now.
# TODO implement something like the tromp taylor pass rule:
# if opponent passed, and we think we're winning then pass as well.

loadsgf sgf/games/2011/UEC/shikousakugo-Fuego.sgf 187
120 reg_genmove w
#? [PASS]*

loadsgf sgf/games/2011/UEC/shikousakugo-Fuego.sgf 197
130 reg_genmove w
#? [PASS]*

loadsgf sgf/games/2011/UEC/shikousakugo-Fuego.sgf 206
140 reg_genmove w
#? [PASS]*
