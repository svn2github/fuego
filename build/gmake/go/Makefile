NAME = fuego_go
DBG_NAME = lib$(NAME).a
REL_NAME = lib$(NAME).a

VPATH = ../../..

DBG_OBJDIR = ../build/obj/$(NAME)/debug
REL_OBJDIR = ../build/obj/$(NAME)/release
DBG_BINDIR = ../build/debug
REL_BINDIR = ../build/release

PROJ_CXXFLAGS = -DUNIX=1 -I../../../gtpengine -I../../../smartgame
PROJ_DBG_CXXFLAGS = $(PROJ_CXXFLAGS)
PROJ_REL_CXXFLAGS = $(PROJ_CXXFLAGS)

PROJ_DBG_LNFLAGS =
PROJ_REL_LNFLAGS =

PROJ_DBG_LIB =
PROJ_REL_LIB =

SRC_CPP= \
  go/GoBensonSolver.cpp \
  go/GoBlock.cpp \
  go/GoBoard.cpp \
  go/GoBoardCheckPerformance.cpp \
  go/GoBoardRestorer.cpp \
  go/GoBoardSynchronizer.cpp \
  go/GoBoardUpdater.cpp \
  go/GoBoardUtil.cpp \
  go/GoBook.cpp \
  go/GoChain.cpp \
  go/GoEyeCount.cpp \
  go/GoEyeUtil.cpp \
  go/GoGame.cpp \
  go/GoGtpCommandUtil.cpp \
  go/GoGtpEngine.cpp \
  go/GoGtpExtraCommands.cpp \
  go/GoGtpTimeSettings.cpp \
  go/GoInfluence.cpp \
  go/GoInit.cpp \
  go/GoKomi.cpp \
  go/GoLadder.cpp \
  go/GoMotive.cpp \
  go/GoNodeUtil.cpp \
  go/GoPlayer.cpp \
  go/GoPlayerMove.cpp \
  go/GoRules.cpp \
  go/GoRegion.cpp \
  go/GoRegionBoard.cpp \
  go/GoRegionUtil.cpp \
  go/GoSafetyCommands.cpp \
  go/GoSafetySolver.cpp \
  go/GoSafetyUtil.cpp \
  go/GoSearch.cpp \
  go/GoStaticLadder.cpp \
  go/GoStaticSafetySolver.cpp \
  go/GoTimeControl.cpp

default : all

include ../Makefile.com.inc
include ../Makefile.lib.inc
