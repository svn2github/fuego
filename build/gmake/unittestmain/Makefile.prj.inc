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

PROJ_CXXFLAGS = -DUNIX=1 -I../../../gtpengine
PROJ_DBG_CXXFLAGS = $(PROJ_CXXFLAGS)
PROJ_REL_CXXFLAGS = $(PROJ_CXXFLAGS)

PROJ_DBG_LNFLAGS = -L../build/debug
PROJ_REL_LNFLAGS = -L../build/release

PROJ_LIB = -lfuego_gtpengine -lboost_unit_test_framework$(BOOST_POSTFIX) -lboost_thread$(BOOST_POSTFIX)
PROJ_DBG_LIB = $(PROJ_LIB)
PROJ_REL_LIB = $(PROJ_LIB)

PROJ_DBG_DEP = \
  ../build/debug/libfuego_gtpengine.a

PROJ_REL_DEP = \
  ../build/release/libfuego_gtpengine.a

default : all

SRC_CPP= \
  gtpengine/test/GtpEngineTest.cpp \
  unittestmain/UnitTestMain.cpp
