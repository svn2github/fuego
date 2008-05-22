#-----------------------------------------------------------------------------
# GNU Makefile: common part for all targets
#
# $Id: Makefile.com.inc,v 1.36 2008/03/04 22:39:39 mmueller Exp $
# $Source: /usr/cvsroot/project_explorer/linux/Makefile.com.inc,v $
#-----------------------------------------------------------------------------

MAKEFILE_DIR=$(dir $(lastword $(MAKEFILE_LIST)))
BOOST_POSTFIX = $(shell $(MAKEFILE_DIR)/boost_postfix.sh)

#------------------------------------------------------------------------------

# C++ compiler and flags.

CXX = g++
COMMON_CXXFLAGS += -Wall -ansi -pedantic
# Use -Wextra only for GCC version > 3
ifneq ($(shell echo __GNUC__ | $(CXX) -E -xc - | tail -n 1), 3)
COMMON_CXXFLAGS += -Wextra
endif
DBG_CXXFLAGS += $(PROJ_DBG_CXXFLAGS) $(COMMON_CXXFLAGS) -g -D_DEBUG=1
REL_CXXFLAGS += $(PROJ_REL_CXXFLAGS) $(COMMON_CXXFLAGS) -g -O3 -fno-math-errno -funroll-loops -DNDEBUG

# C compiler and flags.

CC = gcc
COMMON_CFLAGS += -Wall
DBG_CFLAGS += $(PROJ_DBG_CFLAGS) $(COMMON_CFLAGS) -g
REL_CFLAGS += $(PROJ_REL_CFLAGS) $(COMMON_CFLAGS) -g -O3 -DNDEBUG

# Linker.

LN = $(CXX)
COMMON_LNFLAGS += 
DBG_LNFLAGS += $(PROJ_DBG_LNFLAGS) $(COMMON_LNFLAGS) -g
REL_LNFLAGS += $(PROJ_REL_LNFLAGS) $(COMMON_LNFLAGS) -g

AR = ar

DBG_OBJS = \
  $(patsubst %.cpp, $(DBG_OBJDIR)/%.o, $(SRC_CPP)) \
  $(patsubst %.c, $(DBG_OBJDIR)/%.o, $(SRC_C))
REL_OBJS = \
  $(patsubst %.cpp, $(REL_OBJDIR)/%.o, $(SRC_CPP)) \
  $(patsubst %.c, $(REL_OBJDIR)/%.o, $(SRC_C))

#-----------------------------------------------------------------------------

$(DBG_OBJDIR)/%.o: %.cpp
	@echo "$(notdir $@) (dbg)"
	mkdir -p $(dir $@)
	$(CXX) -c $(DBG_CXXFLAGS) -o $@ $<

$(REL_OBJDIR)/%.o: %.cpp
	@echo "$(notdir $@) (rel)"
	mkdir -p $(dir $@)
	$(CXX) -c $(REL_CXXFLAGS) -o $@ $<

$(DBG_OBJDIR)/%.o: %.c
	@echo "$(notdir $@) (dbg)"
	mkdir -p $(dir $@)
	$(CC) -c $(DBG_CFLAGS) -o $@ $<

$(REL_OBJDIR)/%.o: %.c
	@echo "$(notdir $@) (rel)"
	mkdir -p $(dir $@)
	$(CC) -c $(REL_CFLAGS) -o $@ $<

# Creation of dependency files using the compiler with -MM option and
# sed for postprocessing of its output (including the dependency file in
# the target section and adding the correct path to the object file). 

$(DBG_OBJDIR)/%.d: %.cpp
	@echo "$(notdir $@) (dbg)"
	mkdir -p $(dir $@)
	$(CXX) -MM $(DBG_CXXFLAGS) $< | sed 's#\([-A-Za-z0-9_,]*\)\.o[ :]*#$(patsubst %.d,%.o,$@) $@ : #g' >$@

$(REL_OBJDIR)/%.d: %.cpp
	@echo "$(notdir $@) (rel)"
	mkdir -p $(dir $@)
	$(CXX) -MM $(REL_CXXFLAGS) $< | sed 's#\([-A-Za-z0-9_,]*\)\.o[ :]*#$(patsubst %.d,%.o,$@) $@ : #g' >$@

$(DBG_OBJDIR)/%.d: %.c
	@echo "$(notdir $@) (dbg)"
	mkdir -p $(dir $@)
	$(CC) -MM $(DBG_CFLAGS) $< | sed 's#\([-A-Za-z0-9_,]*\)\.o[ :]*#$(patsubst %.d,%.o,$@) $@ : #g' >$@

$(REL_OBJDIR)/%.d: %.c
	@echo "$(notdir $@) (rel)"
	mkdir -p $(dir $@)
	$(CC) -MM $(REL_CFLAGS) $< | sed 's#\([-A-Za-z0-9_,]*\)\.o[ :]*#$(patsubst %.d,%.o,$@) $@ : #g' >$@

debug: $(DBG_BINDIR)/$(DBG_NAME) debug_post_build

release: $(REL_BINDIR)/$(REL_NAME) release_post_build

all: debug release

# Define post build steps in Makefile.prj.inc if needed.

post_build:

debug_post_build: post_build

release_post_build: post_build

clean:
	@rm -f `find $(DBG_OBJDIR) -name "*.o" -o -name "*.d"`
	@rm -f `find $(REL_OBJDIR) -name "*.o" -o -name "*.d"`
	@rm -f $(DBG_BINDIR)/$(DBG_NAME) $(REL_BINDIR)/$(REL_NAME)

#-----------------------------------------------------------------------------

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(SRC_CPP)),)
-include $(patsubst %.cpp, $(DBG_OBJDIR)/%.d, $(SRC_CPP))
-include $(patsubst %.cpp, $(REL_OBJDIR)/%.d, $(SRC_CPP))
endif
ifneq ($(strip $(SRC_C)),)
-include $(patsubst %.c, $(DBG_OBJDIR)/%.d, $(SRC_C))
-include $(patsubst %.c, $(REL_OBJDIR)/%.d, $(SRC_C))
endif
endif

#-----------------------------------------------------------------------------
