# Define Fugo version using svnversion for development versions.
# Replace this line by release version for releases. Development versions
# should be build by appending .SVN$(shell cd ../../..; svnversion) to the
# last released version
FUEGO_VERSION= 0.1.SVN$(shell cd ../../..; svnversion)
