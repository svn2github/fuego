include ../Makefile.version.inc

NAME = fuego_test
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

PROJ_CXXFLAGS = -DUNIX=1 -DVERSION="$(FUEGO_VERSION)" -I../../../go -I../../../gouct -I../../../gtpengine -I../../../smartgame -I../../../simpleplayers
PROJ_DBG_CXXFLAGS = $(PROJ_CXXFLAGS)
PROJ_REL_CXXFLAGS = $(PROJ_CXXFLAGS)

PROJ_DBG_LNFLAGS = -L../build/debug
PROJ_REL_LNFLAGS = -L../build/release

PROJ_LIB = -lfuego_gouct -lfuego_simpleplayers -lfuego_go -lfuego_smartgame -lfuego_gtpengine -lboost_thread$(BOOST_POSTFIX)
PROJ_DBG_LIB = $(PROJ_LIB)
PROJ_REL_LIB = $(PROJ_LIB)

PROJ_DBG_DEP = \
  ../build/debug/libfuego_go.a \
  ../build/debug/libfuego_gouct.a \
  ../build/debug/libfuego_gtpengine.a \
  ../build/debug/libfuego_smartgame.a \
  ../build/debug/libfuego_simpleplayers.a

PROJ_REL_DEP = \
  ../build/release/libfuego_go.a \
  ../build/release/libfuego_gouct.a \
  ../build/release/libfuego_gtpengine.a \
  ../build/release/libfuego_smartgame.a \
  ../build/release/libfuego_simpleplayers.a

SRC_CPP= \
  fuegotest/FuegoTestMain.cpp \
  fuegotest/FuegoTestEngine.cpp

default : all

# These files need recompilation if FUEGO_VERSION changes
$(DBG_OBJDIR)/fuegotest/FuegoTestMain.o : $(FUEGO_VERSION_TSTAMP)
$(REL_OBJDIR)/fuegotest/FuegoTestMain.o : $(FUEGO_VERSION_TSTAMP)
$(DBG_OBJDIR)/fuegotest/FuegoTestEngine.o : $(FUEGO_VERSION_TSTAMP)
$(REL_OBJDIR)/fuegotest/FuegoTestEngine.o : $(FUEGO_VERSION_TSTAMP)

include ../Makefile.com.inc
include ../Makefile.exe.inc
