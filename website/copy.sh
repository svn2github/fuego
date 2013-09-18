FILES="\
competitions.html \
exhibition-games.html \
favicon.ico \
fuego-screenshot.html \
fuego-screenshot.jpg \
fuego-screenshot-thumb.jpg \
index.html \
mac-install.html \
publications.html \
"

echo "Enter user id for SourceForge:"
read NAME

scp $FILES $NAME,fuego@web.sourceforge.net:/home/groups/f/fu/fuego/htdocs
