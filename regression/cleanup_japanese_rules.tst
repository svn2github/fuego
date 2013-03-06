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
# prevent resigning when testing whether pass is generated at the end.
uct_param_player resign_threshold 0

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
#? [PASS|J6|H6|E8|C9]*
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
#? [K11|L10]*
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

loadsgf sgf/games/2012/KGS/Charles60-FuegoBot.sgf 43
150 reg_genmove b
#? [B8]*

loadsgf sgf/games/2012/KGS/Charles60-FuegoBot.sgf 45
160 reg_genmove b
#? [B8]*
# passing as in the game loses by 0.5

loadsgf sgf/games/2012/KGS/Charles60-FuegoBot-variation.sgf 43
170 reg_genmove b
#? [PASS]
# B+0.5, need to pass to win

loadsgf sgf/games/2011/Fuego-1.1-vim-5-variation.sgf 80
180 reg_genmove w
#? [G6]

loadsgf sgf/games/2011/Fuego-1.1-vim-5-variation.sgf 82
190 reg_genmove w
#? [PASS]

loadsgf sgf/games/2011/Fuego-1.1-vim-5-variation.sgf 84
200 reg_genmove w
#? [PASS]

loadsgf sgf/games/2011/Fuego-1.1-vim-5-variation.sgf 86
210 reg_genmove w
#? [PASS]

loadsgf sgf/games/2011/Fuego-1.1-vim-5-variation.sgf 88
220 reg_genmove w
#? [PASS]
# Not passing here finally loses the game

loadsgf sgf/games/2011/Fuego-1.1-vim-5-variation.sgf 90
230 reg_genmove w
#? [PASS]

loadsgf sgf/games/2011/Fuego-1.1-vim-5-variation.sgf 92
240 reg_genmove w
#? [PASS]*

loadsgf sgf/final_moves/yamato-example2.sgf 21
250 reg_genmove b
#? [PASS]*

251 final_score
#? [B\+0\.5]
# escape characters, + and . have special meaning in regular experessions

loadsgf sgf/final_moves/yamato-example2.sgf 22
260 reg_genmove w
#? [PASS]*

261 final_score
#? [W\+0\.5]

loadsgf sgf/final_moves/yamato-example2.sgf 23
270 reg_genmove b
#? [PASS]*

271 final_score
#? [B\+0\.5]

loadsgf sgf/final_moves/yamato-example2.sgf 24
280 reg_genmove w
#? [PASS]*

281 final_score
#? [W\+0\.5]

loadsgf sgf/final_moves/yamato-example2.sgf 25
290 reg_genmove b
#? [PASS]*

291 final_score
#? [B\+0\.5]

loadsgf sgf/final_moves/yamato-example2.sgf 26
300 reg_genmove w
#? [PASS]*

301 final_score
#? [W\+0\.5]

loadsgf sgf/final_moves/aja-example.sgf 21
310 reg_genmove b
#? [PASS]*

311 final_score
#? [B\+0\.5]

loadsgf sgf/final_moves/aja-example.sgf 22
320 reg_genmove w
#? [PASS]*

321 final_score
#? [W\+0\.5]

loadsgf sgf/final_moves/aja-example.sgf 23
330 reg_genmove b
#? [PASS]*

331 final_score
#? [B\+0\.5]

loadsgf sgf/final_moves/aja-example.sgf 24
340 reg_genmove w
#? [PASS]*

341 final_score
#? [W\+0\.5]

loadsgf sgf/final_moves/aja-example.sgf 25
350 reg_genmove b
#? [PASS]*

351 final_score
#? [B\+0\.5]

loadsgf sgf/final_moves/aja-example.sgf 26
360 reg_genmove w
#? [PASS]*

361 final_score
#? [W\+0\.5]
