#-----------------------------------------------------------------------------
# scoring_static: end-of-game scoring under both Chinese and Japanese rules.
# Test cases where territory (Japanese) rules score differently than 
# area (e.g. Chinese) rules.
# Examples: play dame point or pass, do not play in own territory.
# Also see go/GoRules.h
# Also see http://en.wikipedia.org/wiki/Rules_of_Go#Scoring_systems
#-----------------------------------------------------------------------------

loadsgf sgf/final_moves/aja-example.sgf 22

10 static_score tromp_taylor_score
#? [B\+0\.5]

20 static_score japanese_score
#? [W\+0\.5]*

loadsgf sgf/final_moves/aja-example.sgf 23

30 static_score tromp_taylor_score
#? [B\+0\.5]

40 static_score japanese_score
#? [B\+0\.5]

loadsgf sgf/final_moves/aja-example.sgf 24

50 static_score tromp_taylor_score
#? [B\+0\.5]

60 static_score japanese_score
#? [W\+0\.5]*

loadsgf sgf/final_moves/aja-example.sgf 25

70 static_score tromp_taylor_score
#? [B\+0\.5]

80 static_score japanese_score
#? [B\+0\.5]

loadsgf sgf/final_moves/aja-example.sgf 26

90 static_score tromp_taylor_score
#? [B\+0\.5]

100 static_score japanese_score
#? [W\+0\.5]*

loadsgf sgf/final_moves/pass-scoring.sgf 21

110 static_score tromp_taylor_score
#? [B\+0\.5]

120 static_score japanese_score
#? [B\+0\.5]

loadsgf sgf/final_moves/pass-scoring.sgf 22

130 static_score tromp_taylor_score
#? [B\+0\.5]

140 static_score japanese_score
#? [B\+0\.5]

loadsgf sgf/final_moves/pass-scoring.sgf 23

150 static_score tromp_taylor_score
#? [B\+0\.5]

160 static_score japanese_score
#? [B\+1\.5]*

loadsgf sgf/final_moves/pass-scoring.sgf 24

170 static_score tromp_taylor_score
#? [B\+0\.5]

180 static_score japanese_score
#? [B\+1\.5]*

loadsgf sgf/final_moves/pass-scoring.sgf 25

190 static_score tromp_taylor_score
#? [B\+0\.5]

200 static_score japanese_score
#? [B\+2\.5]*

loadsgf sgf/final_moves/pass-scoring.sgf 26

210 static_score tromp_taylor_score
#? [B\+0\.5]

220 static_score japanese_score
#? [B\+1\.5]*

loadsgf sgf/final_moves/pass-scoring.sgf 27

230 static_score tromp_taylor_score
#? [B\+0\.5]

240 static_score japanese_score
#? [B\+1\.5]*

loadsgf sgf/final_moves/pass-scoring.sgf 28

250 static_score tromp_taylor_score
#? [B\+0\.5]

260 static_score japanese_score
#? [B\+0\.5]

loadsgf sgf/final_moves/pass-scoring.sgf 29

270 static_score tromp_taylor_score
#? [B\+0\.5]

280 static_score japanese_score
#? [B\+0\.5]

loadsgf sgf/final_moves/pass-scoring.sgf 30

290 static_score tromp_taylor_score
#? [B\+0\.5]

300 static_score japanese_score
#? [B\+0\.5]
