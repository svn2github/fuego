#-----------------------------------------------------------------------------
# GNU Makefile: part for static libraries
#-----------------------------------------------------------------------------

MAKEFILE_DIR=$(dir $(lastword $(MAKEFILE_LIST)))

include $(MAKEFILE_DIR)/Makefile.com.inc


$(DBG_BINDIR)/$(DBG_NAME): $(DBG_OBJS)
	@echo "Creating $@"
	mkdir -p $(dir $@)
	$(AR) -csr $@ $(DBG_OBJS)

$(REL_BINDIR)/$(REL_NAME): $(REL_OBJS)
	@echo "Creating $@"
	mkdir -p $(REL_BINDIR)
	$(AR) -csr $@ $(REL_OBJS)
