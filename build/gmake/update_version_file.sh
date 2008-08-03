#!/bin/bash
# Creates or updates a file with the SVN version using the svnversion command.
# The file date will be updated only, if the version has changed or the file
# was newly created, such that the file can be used as a dependency in
# makefiles. The content of the file is the SVN version without trailing
# newline. The script writes the fully qualified path to the file to standard
# output

cd "$(dirname $0)"
FILENAME="$PWD/build/version"
SVNVERSION=$(svnversion -n ../.. || echo "unknown")
if [ ! -e "$FILENAME" ] || [ $(cat "$FILENAME") != "$SVNVERSION" ]; then
    echo -n "$SVNVERSION" > "$FILENAME"
fi
echo "$FILENAME"
