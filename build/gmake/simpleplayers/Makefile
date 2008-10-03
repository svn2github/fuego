NAME = fuego_simpleplayers
DBG_NAME = lib$(NAME).a
REL_NAME = lib$(NAME).a

VPATH = ../../..

DBG_OBJDIR = ../build/obj/$(NAME)/debug
REL_OBJDIR = ../build/obj/$(NAME)/release
DBG_BINDIR = ../build/debug
REL_BINDIR = ../build/release

PROJ_CXXFLAGS = -I../../../go -I../../../gtpengine -I../../../smartgame
PROJ_DBG_CXXFLAGS = $(PROJ_CXXFLAGS)
PROJ_REL_CXXFLAGS = $(PROJ_CXXFLAGS)

PROJ_DBG_LNFLAGS =
PROJ_REL_LNFLAGS =

PROJ_DBG_LIB =
PROJ_REL_LIB =

SRC_CPP= \
  simpleplayers/SpAveragePlayer.cpp \
  simpleplayers/SpCapturePlayer.cpp \
  simpleplayers/SpDumbTacticalPlayer.cpp \
  simpleplayers/SpGreedyPlayer.cpp \
  simpleplayers/SpInfluencePlayer.cpp \
  simpleplayers/SpLadderPlayer.cpp \
  simpleplayers/SpLibertyPlayer.cpp \
  simpleplayers/SpMaxEyePlayer.cpp \
  simpleplayers/SpMinLibPlayer.cpp \
  simpleplayers/SpMoveGenerator.cpp \
  simpleplayers/SpRandomPlayer.cpp \
  simpleplayers/SpSafePlayer.cpp \
  simpleplayers/SpSimplePlayer.cpp \
  simpleplayers/SpUtil.cpp

default : all

include ../Makefile.com.inc
include ../Makefile.lib.inc
