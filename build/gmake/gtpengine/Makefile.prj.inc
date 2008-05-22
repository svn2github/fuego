#-----------------------------------------------------------------------------
# GNU Makefile for static libraries: project dependent part
#
# $Id: Makefile.prj.inc,v 1.37 2007/02/21 18:35:30 emarkus Exp $
# $Source: /usr/cvsroot/project_explorer/linux/gtp/Makefile.prj.inc,v $
#-----------------------------------------------------------------------------

NAME = fuego_gtpengine
DBG_NAME = lib$(NAME).a
REL_NAME = lib$(NAME).a

VPATH = ../../..

DBG_OBJDIR = ../build/obj/$(NAME)/debug
REL_OBJDIR = ../build/obj/$(NAME)/release
DBG_BINDIR = ../build/debug
REL_BINDIR = ../build/release

PROJ_CXXFLAGS = -DUNIX=1
PROJ_DBG_CXXFLAGS = $(PROJ_CXXFLAGS)
PROJ_REL_CXXFLAGS = $(PROJ_CXXFLAGS)

PROJ_DBG_LNFLAGS =
PROJ_REL_LNFLAGS =

PROJ_DBG_LIB =
PROJ_REL_LIB =

default : all

SRC_CPP= \
  gtpengine/GtpEngine.cpp
