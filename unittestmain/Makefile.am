TESTS = fuego_unittest
check_PROGRAMS = $(TESTS)

fuego_unittest_SOURCES = \
@top_srcdir@/go/test/GoBoardTest.cpp \
@top_srcdir@/go/test/GoBoardSynchronizerTest.cpp \
@top_srcdir@/go/test/GoBoardUpdaterTest.cpp \
@top_srcdir@/go/test/GoBoardUtilTest.cpp \
@top_srcdir@/go/test/GoBookTest.cpp \
@top_srcdir@/go/test/GoEyeUtilTest.cpp \
@top_srcdir@/go/test/GoGameTest.cpp \
@top_srcdir@/go/test/GoGameRecordTest.cpp \
@top_srcdir@/go/test/GoGtpCommandUtilTest.cpp \
@top_srcdir@/go/test/GoGtpTimeSettingsTest.cpp \
@top_srcdir@/go/test/GoKomiTest.cpp \
@top_srcdir@/go/test/GoRegionBoardTest.cpp \
@top_srcdir@/go/test/GoTimeControlTest.cpp \
@top_srcdir@/gouct/test/GoUctBoardTest.cpp \
@top_srcdir@/gouct/test/GoUctUtilTest.cpp \
@top_srcdir@/gtpengine/test/GtpEngineTest.cpp \
@top_srcdir@/smartgame/test/SgArrayTest.cpp \
@top_srcdir@/smartgame/test/SgBlackWhiteTest.cpp \
@top_srcdir@/smartgame/test/SgBoardColorTest.cpp \
@top_srcdir@/smartgame/test/SgBoardConstTest.cpp \
@top_srcdir@/smartgame/test/SgBWArrayTest.cpp \
@top_srcdir@/smartgame/test/SgBWSetTest.cpp \
@top_srcdir@/smartgame/test/SgCmdLineOptTest.cpp \
@top_srcdir@/smartgame/test/SgConnCompIteratorTest.cpp \
@top_srcdir@/smartgame/test/SgEBWArrayTest.cpp \
@top_srcdir@/smartgame/test/SgFastLogTest.cpp \
@top_srcdir@/smartgame/test/SgGameReaderTest.cpp \
@top_srcdir@/smartgame/test/SgGtpUtilTest.cpp \
@top_srcdir@/smartgame/test/SgHashTest.cpp \
@top_srcdir@/smartgame/test/SgListTest.cpp \
@top_srcdir@/smartgame/test/SgListUtilityTest.cpp \
@top_srcdir@/smartgame/test/SgMarkerTest.cpp \
@top_srcdir@/smartgame/test/SgMathTest.cpp \
@top_srcdir@/smartgame/test/SgNbIteratorTest.cpp \
@top_srcdir@/smartgame/test/SgNodeTest.cpp \
@top_srcdir@/smartgame/test/SgPointArrayTest.cpp \
@top_srcdir@/smartgame/test/SgPointTest.cpp \
@top_srcdir@/smartgame/test/SgPointSetTest.cpp \
@top_srcdir@/smartgame/test/SgPropTest.cpp \
@top_srcdir@/smartgame/test/SgRectTest.cpp \
@top_srcdir@/smartgame/test/SgRestorerTest.cpp \
@top_srcdir@/smartgame/test/SgSearchTest.cpp \
@top_srcdir@/smartgame/test/SgSListTest.cpp \
@top_srcdir@/smartgame/test/SgStackTest.cpp \
@top_srcdir@/smartgame/test/SgStatisticsTest.cpp \
@top_srcdir@/smartgame/test/SgStrategyTest.cpp \
@top_srcdir@/smartgame/test/SgStringUtilTest.cpp \
@top_srcdir@/smartgame/test/SgSystemTest.cpp \
@top_srcdir@/smartgame/test/SgUctSearchTest.cpp \
@top_srcdir@/smartgame/test/SgUctTreeTest.cpp \
@top_srcdir@/smartgame/test/SgUctTreeUtilTest.cpp \
@top_srcdir@/smartgame/test/SgWriteTest.cpp \
@top_srcdir@/unittestmain/UnitTestMain.cpp

fuego_unittest_CXXFLAGS = -DUNIX=1 $(BOOST_CPPFLAGS)
fuego_unittest_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_UNIT_TEST_FRAMEWORK_LIB) $(BOOST_THREAD_LIB)

fuego_unittest_LDADD = \
@top_srcdir@/gouct/libfuego_gouct.a \
@top_srcdir@/simpleplayers/libfuego_simpleplayers.a \
@top_srcdir@/go/libfuego_go.a \
@top_srcdir@/smartgame/libfuego_smartgame.a \
@top_srcdir@/gtpengine/libfuego_gtpengine.a

fuego_unittest_DEPENDENCIES = \
@top_srcdir@/gouct/libfuego_gouct.a \
@top_srcdir@/simpleplayers/libfuego_simpleplayers.a \
@top_srcdir@/go/libfuego_go.a \
@top_srcdir@/smartgame/libfuego_smartgame.a \
@top_srcdir@/gtpengine/libfuego_gtpengine.a

INCLUDES = -I@top_srcdir@/gtpengine -I@top_srcdir@/smartgame -I@top_srcdir@/go -I@top_srcdir@/simpleplayers -I@top_srcdir@/gouct

DISTCLEANFILES = *~
