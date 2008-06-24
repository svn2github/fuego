#-----------------------------------------------------------------------------
# GNU Makefile for executables: project dependent part
#-----------------------------------------------------------------------------

NAME = fuego_unittest
DBG_NAME = $(NAME)
REL_NAME = $(NAME)

VPATH = ../../..

DBG_OBJDIR = ../build/obj/$(NAME)/debug
REL_OBJDIR = ../build/obj/$(NAME)/release
DBG_BINDIR = ../build/debug
REL_BINDIR = ../build/release

PROJ_CXXFLAGS = -DUNIX=1 -I../../../go -I../../../gouct -I../../../gtpengine -I../../../smartgame
PROJ_DBG_CXXFLAGS = $(PROJ_CXXFLAGS)
PROJ_REL_CXXFLAGS = $(PROJ_CXXFLAGS)

PROJ_DBG_LNFLAGS = -L../build/debug
PROJ_REL_LNFLAGS = -L../build/release

PROJ_LIB = -lfuego_gouct -lfuego_go -lfuego_smartgame -lfuego_gtpengine -lboost_unit_test_framework$(BOOST_POSTFIX) -lboost_thread$(BOOST_POSTFIX)
PROJ_DBG_LIB = $(PROJ_LIB)
PROJ_REL_LIB = $(PROJ_LIB)

PROJ_DBG_DEP = \
  ../build/debug/libfuego_go.a \
  ../build/debug/libfuego_gouct.a \
  ../build/debug/libfuego_gtpengine.a \
  ../build/debug/libfuego_smartgame.a

PROJ_REL_DEP = \
  ../build/release/libfuego_go.a \
  ../build/release/libfuego_gouct.a \
  ../build/release/libfuego_gtpengine.a \
  ../build/release/libfuego_smartgame.a

default : all

SRC_CPP= \
  go/test/GoBoardTest.cpp \
  go/test/GoBoardSynchronizerTest.cpp \
  go/test/GoBoardUpdaterTest.cpp \
  go/test/GoBoardUtilTest.cpp \
  go/test/GoBookTest.cpp \
  go/test/GoEyeUtilTest.cpp \
  go/test/GoGameTest.cpp \
  go/test/GoGameRecordTest.cpp \
  go/test/GoGtpCommandUtilTest.cpp \
  go/test/GoGtpTimeSettingsTest.cpp \
  go/test/GoKomiTest.cpp \
  go/test/GoRegionBoardTest.cpp \
  go/test/GoTimeControlTest.cpp \
  gouct/test/GoUctBoardTest.cpp \
  gouct/test/GoUctUtilTest.cpp \
  gtpengine/test/GtpEngineTest.cpp \
  smartgame/test/SgArrayTest.cpp \
  smartgame/test/SgBlackWhiteTest.cpp \
  smartgame/test/SgBoardColorTest.cpp \
  smartgame/test/SgBoardConstTest.cpp \
  smartgame/test/SgBWArrayTest.cpp \
  smartgame/test/SgBWSetTest.cpp \
  smartgame/test/SgCmdLineOptTest.cpp \
  smartgame/test/SgConnCompIteratorTest.cpp \
  smartgame/test/SgEBWArrayTest.cpp \
  smartgame/test/SgFastLogTest.cpp \
  smartgame/test/SgGameReaderTest.cpp \
  smartgame/test/SgGtpUtilTest.cpp \
  smartgame/test/SgHashTest.cpp \
  smartgame/test/SgListTest.cpp \
  smartgame/test/SgListUtilityTest.cpp \
  smartgame/test/SgMarkerTest.cpp \
  smartgame/test/SgMathTest.cpp \
  smartgame/test/SgNbIteratorTest.cpp \
  smartgame/test/SgNodeTest.cpp \
  smartgame/test/SgPointArrayTest.cpp \
  smartgame/test/SgPointTest.cpp \
  smartgame/test/SgPointSetTest.cpp \
  smartgame/test/SgPropTest.cpp \
  smartgame/test/SgRectTest.cpp \
  smartgame/test/SgRestorerTest.cpp \
  smartgame/test/SgSearchTest.cpp \
  smartgame/test/SgSListTest.cpp \
  smartgame/test/SgStackTest.cpp \
  smartgame/test/SgStatisticsTest.cpp \
  smartgame/test/SgStrategyTest.cpp \
  smartgame/test/SgStringUtilTest.cpp \
  smartgame/test/SgSystemTest.cpp \
  smartgame/test/SgUctSearchTest.cpp \
  smartgame/test/SgUctTreeTest.cpp \
  smartgame/test/SgUctTreeUtilTest.cpp \
  smartgame/test/SgWriteTest.cpp \
  unittestmain/UnitTestMain.cpp
