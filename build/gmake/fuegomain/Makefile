NAME = fuego
DBG_NAME = $(NAME)
REL_NAME = $(NAME)

VPATH = ../../..

# Define Fugo version using svnversion for development versions.
# Replace this line by release version for releases. Development versions
# should be build by appending .SVN$(shell cd ../../..; svnversion) to the
# last released version
FUEGO_VERSION= 0.1.SVN$(shell cd ../../..; svnversion)

DBG_OBJDIR = ../build/obj/$(NAME)/debug
REL_OBJDIR = ../build/obj/$(NAME)/release
DBG_BINDIR = ../build/debug
REL_BINDIR = ../build/release

PROJ_CXXFLAGS = -DUNIX=1 -DVERSION="$(FUEGO_VERSION)" -I../../../go -I../../../gouct -I../../../gtpengine -I../../../smartgame
PROJ_DBG_CXXFLAGS = $(PROJ_CXXFLAGS)
PROJ_REL_CXXFLAGS = $(PROJ_CXXFLAGS)

PROJ_DBG_LNFLAGS = -L../build/debug
PROJ_REL_LNFLAGS = -L../build/release

PROJ_LIB = -lfuego_gouct -lfuego_go -lfuego_smartgame -lfuego_gtpengine -lboost_thread$(BOOST_POSTFIX)
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

SRC_CPP= \
  fuegomain/FuegoMain.cpp \
  fuegomain/FuegoMainEngine.cpp \
  fuegomain/FuegoMainUtil.cpp

default : all

include ../Makefile.com.inc
include ../Makefile.exe.inc
