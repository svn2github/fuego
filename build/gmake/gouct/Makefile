NAME = fuego_gouct
DBG_NAME = lib$(NAME).a
REL_NAME = lib$(NAME).a

VPATH = ../../..

DBG_OBJDIR = ../build/obj/$(NAME)/debug
REL_OBJDIR = ../build/obj/$(NAME)/release
DBG_BINDIR = ../build/debug
REL_BINDIR = ../build/release

PROJ_CXXFLAGS = -DUNIX=1 -I../../../go -I../../../gtpengine -I../../../smartgame
PROJ_DBG_CXXFLAGS = $(PROJ_CXXFLAGS)
PROJ_REL_CXXFLAGS = $(PROJ_CXXFLAGS)

PROJ_DBG_LNFLAGS =
PROJ_REL_LNFLAGS =

PROJ_DBG_LIB =
PROJ_REL_LIB =

SRC_CPP= \
  gouct/GoUctBoard.cpp \
  gouct/GoUctCommands.cpp \
  gouct/GoUctDefaultPlayoutPolicy.cpp \
  gouct/GoUctDefaultPriorKnowledge.cpp \
  gouct/GoUctDefaultRootFilter.cpp \
  gouct/GoUctEstimatorStat.cpp \
  gouct/GoUctGlobalSearch.cpp \
  gouct/GoUctGlobalSearchPlayer.cpp \
  gouct/GoUctObjectWithSearch.cpp \
  gouct/GoUctPatterns.cpp \
  gouct/GoUctRootFilter.cpp \
  gouct/GoUctSearch.cpp \
  gouct/GoUctUtil.cpp

default : all

include ../Makefile.com.inc
include ../Makefile.lib.inc
