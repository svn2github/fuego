#------------------------------------------------------------------------------
# GNU Makefile: part for static libraries
#
# $Id: Makefile.lib.inc,v 1.5 2004/03/15 21:14:21 emarkus Exp $
# $Source: /usr/cvsroot/project_explorer/linux/Makefile.lib.inc,v $
#------------------------------------------------------------------------------

$(DBG_BINDIR)/$(DBG_NAME): $(DBG_OBJS)
	@echo "Creating $@"
	mkdir -p $(dir $@)
	$(AR) -csr $@ $(DBG_OBJS)

$(REL_BINDIR)/$(REL_NAME): $(REL_OBJS)
	@echo "Creating $@"
	mkdir -p $(REL_BINDIR)
	$(AR) -csr $@ $(REL_OBJS)
