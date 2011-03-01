#------------------------------------------------------------------------------
# Static safety test for fullboard positions
# 1~31 (q1-q31 from set1)
# 32~58 (CZD professional games from set2)
# 59~94   (korean professional players' games from set3)
# Total: 94*3 = 282 tests
# All results are based on improved benson function (recursively finding 2-path
# for all interior points.
#
# 
#------------------------------------------------------------------------------

loadsgf sgf/safetytest-whole-board/q1.sgf

10 go_safe benson
#? [119 .* .*] 

11 go_safe benson black
#? [53 .* .*]

12 go_safe benson white
#? [66 .* .*]

loadsgf sgf/safetytest-whole-board/q2.sgf

20 go_safe benson
#? [0]

21 go_safe benson black
#? [0]

22 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q3.sgf

30 go_safe benson
#? [57 .*]

31 go_safe benson black
#? [19 .*]

32 go_safe benson white
#? [38 .*]

loadsgf sgf/safetytest-whole-board/q4.sgf

40 go_safe benson
#? [265 .*]

41 go_safe benson black
#? [115 .*]

42 go_safe benson white
#? [150 .*]

loadsgf sgf/safetytest-whole-board/q5.sgf

50 go_safe benson
#? [28 .*]

51 go_safe benson black
#? [0]

52 go_safe benson white
#? [28 .*]

loadsgf sgf/safetytest-whole-board/q6.sgf

60 go_safe benson
#? [19 .*]

61 go_safe benson black
#? [0]

62 go_safe benson white
#? [19 .*]

loadsgf sgf/safetytest-whole-board/q7.sgf

70 go_safe benson
#? [0]

71 go_safe benson black
#? [0]

72 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q8.sgf

80 go_safe benson
#? [25 .*]

81 go_safe benson black
#? [0]

82 go_safe benson white
#? [25 .*]

loadsgf sgf/safetytest-whole-board/q9.sgf

90 go_safe benson
#? [0]

91 go_safe benson black
#? [0]

92 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q10.sgf

100 go_safe benson
#? [0]

101 go_safe benson black
#? [0]

102 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q11.sgf

110 go_safe benson
#? [30 .*]

111 go_safe benson black
#? [0]

112 go_safe benson white
#? [30 .*]

loadsgf sgf/safetytest-whole-board/q12.sgf

120 go_safe benson
#? [0]

121 go_safe benson black
#? [0]

122 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q13.sgf

130 go_safe benson
#? [81 .*]

131 go_safe benson black
#? [49 .*]

132 go_safe benson white
#? [32 .*]

loadsgf sgf/safetytest-whole-board/q14.sgf

140 go_safe benson
#? [210 .*]

141 go_safe benson black
#? [87 .*]

142 go_safe benson white
#? [123 .*]

loadsgf sgf/safetytest-whole-board/q15.sgf

150 go_safe benson
#? [91 .*]

151 go_safe benson black
#? [49 .*]

152 go_safe benson white
#? [42 .*]

loadsgf sgf/safetytest-whole-board/q16.sgf

160 go_safe benson
#? [55 .*]

161 go_safe benson black
#? [55 .*]

162 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q17.sgf

170 go_safe benson
#? [48 .*]

171 go_safe benson black
#? [48 .*]

172 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q18.sgf

180 go_safe benson
#? [0]

181 go_safe benson black
#? [0]

182 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q19.sgf

190 go_safe benson
#? [16 .*]

191 go_safe benson black
#? [0]

192 go_safe benson white
#? [16 .*]

loadsgf sgf/safetytest-whole-board/q20.sgf

200 go_safe benson
#? [0]

201 go_safe benson black
#? [0]

202 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q21.sgf

210 go_safe benson
#? [54 .*]

211 go_safe benson black
#? [54 .*]

212 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q22.sgf

220 go_safe benson
#? [138 .*]

221 go_safe benson black
#? [66 .*]

222 go_safe benson white
#? [72 .*]

loadsgf sgf/safetytest-whole-board/q23.sgf

230 go_safe benson
#? [157 .*]

231 go_safe benson black
#? [92 .*]

232 go_safe benson white
#? [65 .*]

loadsgf sgf/safetytest-whole-board/q24.sgf

240 go_safe benson
#? [117 .*]

241 go_safe benson black
#? [75 .*]

242 go_safe benson white
#? [42 .*]

loadsgf sgf/safetytest-whole-board/q25.sgf

250 go_safe benson
#? [0]

251 go_safe benson black
#? [0]

252 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q26.sgf

260 go_safe benson
#? [125 .*]

261 go_safe benson black
#? [39 .*]

262 go_safe benson white
#? [86 .*]

loadsgf sgf/safetytest-whole-board/q27.sgf

270 go_safe benson
#? [0]

271 go_safe benson black
#? [0]

272 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q28.sgf

280 go_safe benson
#? [0]

281 go_safe benson black
#? [0]

282 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q29.sgf

290 go_safe benson
#? [136 .*]

291 go_safe benson black
#? [75 .*]

292 go_safe benson white
#? [61 .*]

loadsgf sgf/safetytest-whole-board/q30.sgf

300 go_safe benson
#? [47 .*]

301 go_safe benson black
#? [47 .*]

302 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/q31.sgf

310 go_safe benson
#? [68 .*]

311 go_safe benson black
#? [25 .*]

312 go_safe benson white
#? [43 .*]

loadsgf sgf/safetytest-whole-board/CZD_05.sgf

320 go_safe benson
#? [0]

321 go_safe benson black
#? [0]

322 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/CZD_16.sgf

330 go_safe benson
#? [127 .*]

331 go_safe benson black
#? [84 .*]

332 go_safe benson white
#? [43 .*]

loadsgf sgf/safetytest-whole-board/CZD_17.sgf

340 go_safe benson
#? [31 .*]

341 go_safe benson black
#? [31 .*]

342 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/CZD_20.sgf

350 go_safe benson
#? [34 .*]

351 go_safe benson black
#? [34 .*]

352 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/CZD_21.sgf

360 go_safe benson
#? [193 .*]

361 go_safe benson black
#? [74 .*]

362 go_safe benson white
#? [119 .*]

loadsgf sgf/safetytest-whole-board/CZD_24.sgf

370 go_safe benson
#? [0]

371 go_safe benson black
#? [0]

372 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/CZD_26.sgf

380 go_safe benson
#? [34 .*]

381 go_safe benson black
#? [34 .*]

382 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/CZD_28.sgf

390 go_safe benson
#? [34 .*]

391 go_safe benson black
#? [0]

392 go_safe benson white
#? [34 .*]

loadsgf sgf/safetytest-whole-board/CZD_29.sgf

400 go_safe benson
#? [25 .*]

401 go_safe benson black
#? [0]

402 go_safe benson white
#? [25 .*]

loadsgf sgf/safetytest-whole-board/CZD_31.sgf

410 go_safe benson
#? [71 .*]

411 go_safe benson black
#? [0]

412 go_safe benson white
#? [71 .*]

loadsgf sgf/safetytest-whole-board/CZD_33.sgf

420 go_safe benson
#? [0]

421 go_safe benson black
#? [0]

422 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/CZD_34.sgf

430 go_safe benson
#? [86 .*]

431 go_safe benson black
#? [61 .*]

432 go_safe benson white
#? [25 .*]

loadsgf sgf/safetytest-whole-board/CZD_35.sgf

440 go_safe benson
#? [80 .*]

441 go_safe benson black
#? [0]

442 go_safe benson white
#? [80 .*]

loadsgf sgf/safetytest-whole-board/CZD_36.sgf

450 go_safe benson
#? [24 .*]

451 go_safe benson black
#? [24 .*]

452 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/CZD_39.sgf

460 go_safe benson
#? [43 .*]

461 go_safe benson black
#? [0]

462 go_safe benson white
#? [43 .*]

loadsgf sgf/safetytest-whole-board/CZD_41.sgf

470 go_safe benson
#? [76 .*]

471 go_safe benson black
#? [76 .*]

472 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/CZD_46.sgf

480 go_safe benson
#? [23 .*]

481 go_safe benson black
#? [23 .*]

482 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/CZD_55.sgf

490 go_safe benson
#? [78 .*]

491 go_safe benson black
#? [61 .*]

492 go_safe benson white
#? [17 .*]

loadsgf sgf/safetytest-whole-board/CZD_58.sgf

500 go_safe benson
#? [47 .*]

501 go_safe benson black
#? [22 .*]

502 go_safe benson white
#? [25 .*]

loadsgf sgf/safetytest-whole-board/CZD_64.sgf

510 go_safe benson
#? [0]

511 go_safe benson black
#? [0]

512 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/CZD_66.sgf

520 go_safe benson
#? [14 .*]

521 go_safe benson black
#? [0]

522 go_safe benson white
#? [14 .*]

loadsgf sgf/safetytest-whole-board/CZD_67.sgf

530 go_safe benson
#? [115 .*]

531 go_safe benson black
#? [54 .*]

532 go_safe benson white
#? [61 .*]

loadsgf sgf/safetytest-whole-board/CZD_68.sgf

540 go_safe benson
#? [91 .*]

541 go_safe benson black
#? [28 .*]

542 go_safe benson white
#? [63 .*]

loadsgf sgf/safetytest-whole-board/CZD_69.sgf

550 go_safe benson
#? [59 .*]

551 go_safe benson black
#? [59 .*]

552 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/CZD_71.sgf

560 go_safe benson
#? [0]

561 go_safe benson black
#? [0]

562 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/CZD_72.sgf

570 go_safe benson
#? [0]

571 go_safe benson black
#? [0]

572 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/CZD_79.sgf

580 go_safe benson
#? [44 .*]

581 go_safe benson black
#? [0]

582 go_safe benson white
#? [44 .*]

loadsgf sgf/safetytest-whole-board/k01.sgf

590 go_safe benson
#? [0]

591 go_safe benson black
#? [0]

592 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k02.sgf

600 go_safe benson
#? [186 .*]

601 go_safe benson black
#? [81 .*]

602 go_safe benson white
#? [105 .*]

loadsgf sgf/safetytest-whole-board/k03.sgf

610 go_safe benson
#? [133 .*]

611 go_safe benson black
#? [72 .*]

612 go_safe benson white
#? [61 .*]

loadsgf sgf/safetytest-whole-board/k04.sgf

620 go_safe benson
#? [52 .*]

621 go_safe benson black
#? [0]

622 go_safe benson white
#? [52 .*]

loadsgf sgf/safetytest-whole-board/k05.sgf

630 go_safe benson
#? [49 .*]

631 go_safe benson black
#? [0]

632 go_safe benson white
#? [49 .*]

loadsgf sgf/safetytest-whole-board/k06.sgf

640 go_safe benson
#? [0]

641 go_safe benson black
#? [0]

642 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k07.sgf

650 go_safe benson
#? [93 .*]

651 go_safe benson black
#? [69 .*]

652 go_safe benson white
#? [24 .*]

loadsgf sgf/safetytest-whole-board/k08.sgf

660 go_safe benson
#? [0]

661 go_safe benson black
#? [0]

662 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k09.sgf

670 go_safe benson
#? [33 .*]

671 go_safe benson black
#? [0]

672 go_safe benson white
#? [33 .*]

loadsgf sgf/safetytest-whole-board/k10.sgf

680 go_safe benson
#? [0]

681 go_safe benson black
#? [0]

682 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k11.sgf

690 go_safe benson
#? [0]

691 go_safe benson black
#? [0]

692 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k12.sgf

700 go_safe benson
#? [62 .*]

701 go_safe benson black
#? [0]

702 go_safe benson white
#? [62 .*]

loadsgf sgf/safetytest-whole-board/k13.sgf

710 go_safe benson
#? [70 .*]

711 go_safe benson black
#? [0]

712 go_safe benson white
#? [70 .*]

loadsgf sgf/safetytest-whole-board/k14.sgf

720 go_safe benson
#? [43 .*]

721 go_safe benson black
#? [43 .*]

722 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k15.sgf

730 go_safe benson
#? [81 .*]

731 go_safe benson black
#? [40 .*]

732 go_safe benson white
#? [41 .*]

loadsgf sgf/safetytest-whole-board/k16.sgf

740 go_safe benson
#? [0]

741 go_safe benson black
#? [0]

742 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k17.sgf

750 go_safe benson
#? [18 .*]

751 go_safe benson black
#? [18 .*]

752 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k18.sgf

760 go_safe benson
#? [36 .*]

761 go_safe benson black
#? [0]

762 go_safe benson white
#? [36 .*]

loadsgf sgf/safetytest-whole-board/k19.sgf

770 go_safe benson
#? [0]

771 go_safe benson black
#? [0]

772 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k20.sgf

780 go_safe benson
#? [11 .*]

781 go_safe benson black
#? [11 .*]

782 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k21.sgf

790 go_safe benson
#? [12 .*]

791 go_safe benson black
#? [0]

792 go_safe benson white
#? [12 .*]

loadsgf sgf/safetytest-whole-board/k22.sgf

800 go_safe benson
#? [20 .*]

801 go_safe benson black
#? [0]

802 go_safe benson white
#? [20 .*]

loadsgf sgf/safetytest-whole-board/k23.sgf

810 go_safe benson
#? [0]

811 go_safe benson black
#? [0]

812 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k24.sgf

820 go_safe benson
#? [0]

821 go_safe benson black
#? [0]

822 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k25.sgf

830 go_safe benson
#? [0]

831 go_safe benson black
#? [0]

832 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k26.sgf

840 go_safe benson
#? [32 .*]

841 go_safe benson black
#? [32 .*]

842 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k27.sgf

850 go_safe benson
#? [120 .*]

851 go_safe benson black
#? [66 .*]

852 go_safe benson white
#? [54 .*]

loadsgf sgf/safetytest-whole-board/k28.sgf

860 go_safe benson
#? [115 .*]

861 go_safe benson black
#? [22 .*]

862 go_safe benson white
#? [93 .*]

loadsgf sgf/safetytest-whole-board/k29.sgf

870 go_safe benson
#? [0]

871 go_safe benson black
#? [0]

872 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k30.sgf

880 go_safe benson
#? [0]

881 go_safe benson black
#? [0]

882 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k31.sgf

890 go_safe benson
#? [144 .*]

891 go_safe benson black
#? [68 .*]

892 go_safe benson white
#? [76 .*]

loadsgf sgf/safetytest-whole-board/k32.sgf

900 go_safe benson
#? [12 .*]

901 go_safe benson black
#? [12 .*]

902 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k33.sgf

910 go_safe benson
#? [0]

911 go_safe benson black
#? [0]

912 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k34.sgf

920 go_safe benson
#? [0]

921 go_safe benson black
#? [0]

922 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k34.sgf

930 go_safe benson
#? [0]

931 go_safe benson black
#? [0]

932 go_safe benson white
#? [0]

loadsgf sgf/safetytest-whole-board/k35.sgf

940 go_safe benson
#? [0]

941 go_safe benson black
#? [0]

942 go_safe benson white
#? [0]


