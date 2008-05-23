#-----------------------------------------------------------------------------
# Main Makefile for project_explorer.
# The sub-makefiles are invoked sequentially
# (see: Peter Miller: Recursive Make Considered Harmful
# http://aegis.sourceforge.net/auug97.pdf)
#-----------------------------------------------------------------------------

# All projects to build in this order.
PROJECTS = \
  gtpengine \
  smartgame

# sequentially to avoid same sub-target in sub-make invoked twice
.PHONY: default
default:
	$(MAKE) debug
	$(MAKE) release

.PHONY: debug
debug:
	@for dir in $(PROJECTS); do \
           if test -f $$dir/Makefile; then \
             echo -e "\n--------------- $$dir (dbg)"; \
             $(MAKE) -C $$dir debug || exit -1; \
           fi; \
        done
	@echo ""

.PHONY: release
release:
	@for dir in $(PROJECTS); do \
           if test -f $$dir/Makefile; then \
             echo -e "\n--------------- $$dir (rel)"; \
             $(MAKE) -C $$dir release || exit -1; \
           fi; \
        done
	@echo ""

.PHONY: clean
clean:
	@for dir in $(PROJECTS); do \
           if test -f $$dir/Makefile; then \
             echo -e "\n--------------- Removing temporary files for $$dir"; \
             $(MAKE) -C $$dir clean || exit -1; \
           fi; \
         done
	@echo ""
	$(MAKE) -C test clean

.PHONY: tags
tags:
	@echo -e "Generating tags for Emacs"
	@(etags `find ../.. -name "*.h" -o -name "*.cpp"`)

.PHONY: test
test: test-debug

.PHONY: test-all
test-all: test-debug test-release

.PHONY: test-debug
test-debug: debug
	@echo -e "\n--------------- test (dbg)"; \
	$(MAKE) -C unittestmain run-debug

.PHONY: test-release
test-release: release
	@echo -e "\n--------------- test (rel)"; \
	$(MAKE) -C unittestmain run-release
