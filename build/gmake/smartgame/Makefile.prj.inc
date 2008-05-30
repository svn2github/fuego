#-----------------------------------------------------------------------------
# GNU Makefile for static libraries: project dependent part
#-----------------------------------------------------------------------------

NAME = fuego_smartgame
DBG_NAME = lib$(NAME).a
REL_NAME = lib$(NAME).a

VPATH = ../../..

DBG_OBJDIR = ../build/obj/$(NAME)/debug
REL_OBJDIR = ../build/obj/$(NAME)/release
DBG_BINDIR = ../build/debug
REL_BINDIR = ../build/release

PROJ_CXXFLAGS = -DUNIX=1 -I../../../gtpengine
PROJ_DBG_CXXFLAGS = $(PROJ_CXXFLAGS)
PROJ_REL_CXXFLAGS = $(PROJ_CXXFLAGS)

PROJ_DBG_LNFLAGS =
PROJ_REL_LNFLAGS =

PROJ_DBG_LIB =
PROJ_REL_LIB =

default : all

SRC_CPP= \
  smartgame/SgBoardConst.cpp \
  smartgame/SgCmdLineOpt.cpp \
  smartgame/SgConnCompIterator.cpp \
  smartgame/SgDebug.cpp \
  smartgame/SgEvaluatedMoves.cpp \
  smartgame/SgException.cpp \
  smartgame/SgFastLog.cpp \
  smartgame/SgGameReader.cpp \
  smartgame/SgGameWriter.cpp \
  smartgame/SgGtpClient.cpp \
  smartgame/SgGtpCommands.cpp \
  smartgame/SgGtpUtil.cpp \
  smartgame/SgIncrementalStack.cpp \
  smartgame/SgInit.cpp \
  smartgame/SgListUtility.cpp \
  smartgame/SgMemCheck.cpp \
  smartgame/SgMiaiMap.cpp \
  smartgame/SgMiaiStrategy.cpp \
  smartgame/SgNbIterator.cpp \
  smartgame/SgNode.cpp \
  smartgame/SgNodeUtil.cpp \
  smartgame/SgPoint.cpp \
  smartgame/SgPointSet.cpp \
  smartgame/SgPointSetUtil.cpp \
  smartgame/SgProcess.cpp \
  smartgame/SgProp.cpp \
  smartgame/SgRandom.cpp \
  smartgame/SgRect.cpp \
  smartgame/SgSearch.cpp \
  smartgame/SgSearchStatistics.cpp \
  smartgame/SgStrategy.cpp \
  smartgame/SgStringUtil.cpp \
  smartgame/SgSystem.cpp \
  smartgame/SgTime.cpp \
  smartgame/SgTimeControl.cpp \
  smartgame/SgTimeRecord.cpp \
  smartgame/SgUctBiasTermPrecomp.cpp \
  smartgame/SgUctPriorKnowledgeEven.cpp \
  smartgame/SgUctSearch.cpp \
  smartgame/SgUctTree.cpp \
  smartgame/SgUctTreeUtil.cpp \
  smartgame/SgWrite.cpp
