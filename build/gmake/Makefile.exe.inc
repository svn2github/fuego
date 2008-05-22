#------------------------------------------------------------------------------
# GNU Makefile: part for executables
#------------------------------------------------------------------------------

# Rules for linking

$(DBG_BINDIR)/$(DBG_NAME): $(DBG_OBJS) $(PROJ_DBG_DEP)
	@echo "Linking $@"
	mkdir -p $(dir $@)
	$(LN) -o $@ $(DBG_LNFLAGS) $(DBG_OBJS) $(PROJ_DBG_LIB)

$(REL_BINDIR)/$(REL_NAME): $(REL_OBJS) $(PROJ_REL_DEP)
	@echo "Linking $@"
	mkdir -p $(REL_BINDIR)
	$(LN) -o $@ $(REL_LNFLAGS) $(REL_OBJS) $(PROJ_REL_LIB)
