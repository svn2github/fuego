# Set to 1 for releases, 0 for unreleased development versions
FUEGO_IS_RELEASE=1

# Set to release version for releases or version of last release in this
# branch for unreleased development versions
FUEGO_RELEASE_VERSION=0.2

# FUEGO_VERSION_FILE (which might be empty) should be used as a dependency
# for targets that use FUEGO_VERSION

ifeq ($(FUEGO_IS_RELEASE), 1)
FUEGO_VERSION_FILE =
FUEGO_VERSION=$(FUEGO_RELEASE_VERSION)
else
MAKEFILE_DIR=$(dir $(lastword $(MAKEFILE_LIST)))
FUEGO_VERSION_FILE = $(shell $(MAKEFILE_DIR)update_version_file.sh)
FUEGO_VERSION = "$(FUEGO_RELEASE_VERSION).SVN`cat $(FUEGO_VERSION_FILE)`"
endif
