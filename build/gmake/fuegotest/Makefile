include ../Makefile.version.inc

NAME = fuego_test
DBG_NAME = $(NAME)
REL_NAME = $(NAME)

VPATH = ../../..

DBG_OBJDIR = ../build/obj/$(NAME)/debug
REL_OBJDIR = ../build/obj/$(NAME)/release
DBG_BINDIR = ../build/debug
REL_BINDIR = ../build/release

PROJ_CXXFLAGS = -DUNIX=1 -DVERSION=\"$(FUEGO_VERSION)\" -I../../../go -I../../../gouct -I../../../gtpengine -I../../../smartgame -I../../../simpleplayers
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

# These files need to be rebuilt if the version changes
$(DBG_OBJDIR)/fuegotest/FuegoTestEngine.o: $(FUEGO_VERSION_FILE)
$(REL_OBJDIR)/fuegotest/FuegoTestEngine.o: $(FUEGO_VERSION_FILE)

default : all

include ../Makefile.com.inc
include ../Makefile.exe.inc
