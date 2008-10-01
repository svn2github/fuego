FILES="\
favicon.ico \
fuego-screenshot.html \
fuego-screenshot.jpg \
fuego-screenshot-thumb.jpg \
index.html \
"

echo "Enter user id for SourceForge:"
read NAME

scp $FILES $NAME,gogui@web.sourceforge.net:/home/groups/f/fu/fuego/htdocs
