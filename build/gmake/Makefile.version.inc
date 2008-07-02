#-----------------------------------------------------------------------------
# Creates or updates a file and make variable with the SVN version of
# Fuego.
# The file is FUEGO_VERSION_TSTAMP, the variable name FUEGO_VERSION
# The time of the last modification of the file will change whenever the
# SVN version changes, so it can be used as a dependency for targets that
# need to be rebuilt, when the value of the variable changes.
#-----------------------------------------------------------------------------

MAKEFILE_DIR=$(dir $(lastword $(MAKEFILE_LIST)))
FUEGO_VERSION_TSTAMP=$(MAKEFILE_DIR)/build/version
FUEGO_VERSION=$(shell svnversion $(MAKEFILE_DIR)/../..)
FUEGO_VERSION_UPDATED=\
  $(shell test "`cat $(FUEGO_VERSION_TSTAMP)`" == "$(FUEGO_VERSION)" \
  || (echo "$(FUEGO_VERSION)" > $(FUEGO_VERSION_TSTAMP) && echo "1"))
