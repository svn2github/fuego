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

PROJ_CXXFLAGS = -DUNIX=1 -I../../../gtpengine -I../../../smartgame
PROJ_DBG_CXXFLAGS = $(PROJ_CXXFLAGS)
PROJ_REL_CXXFLAGS = $(PROJ_CXXFLAGS)

PROJ_DBG_LNFLAGS = -L../build/debug
PROJ_REL_LNFLAGS = -L../build/release

PROJ_LIB = -lfuego_smartgame -lfuego_gtpengine -lboost_unit_test_framework$(BOOST_POSTFIX) -lboost_thread$(BOOST_POSTFIX)
PROJ_DBG_LIB = $(PROJ_LIB)
PROJ_REL_LIB = $(PROJ_LIB)

PROJ_DBG_DEP = \
  ../build/debug/libfuego_gtpengine.a \
  ../build/debug/libfuego_smartgame.a

PROJ_REL_DEP = \
  ../build/release/libfuego_gtpengine.a \
  ../build/release/libfuego_smartgame.a

default : all

SRC_CPP= \
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
