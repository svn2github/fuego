#!/bin/bash

# Script to do daily tasks for Fuego.
#
# This script should be run as a nightly cron job by one of the Fuego admins.
# Since SourceForge discourages auto-updaters, it should *not* be run on the
# SourceForge shell server, but on a private machine.
# To run it, check out a version of fuego to a dedicated directory, for
# example ~/fuego-daily (don't use this checked out version for other tasks).
# Then register ~/fuego-daily/fuego/tools/cronjob/fuego-daily.sh as a
# cron job to run once a day.
# You need to have set up password-less ssh to fuego.sourceforge.net
# (see http://alexandria.wiki.sourceforge.net/SSH+Key+Generation)

FUEGO_DIR=$(dirname $0)/../..
FUEGO_WEB_SERVER=fuego.sourceforge.net
FUEGO_WEB_DIR=/home/groups/f/fu/fuego/htdocs

# Update the checked out sources.
# Exit immediately if nothing was changed (svn status code U, A, D did not
# occur).

cd $FUEGO_DIR
svn update | awk '/[UAD] /{exit 1}' && exit

# Generate documentation and put it on the web server

cd $FUEGO_DIR/doc
make || exit 1
ssh $FUEGO_WEB_SERVER mkdir -p $FUEGO_WEB_DIR/doc/daily
rsync -r fuego-doc $FUEGO_WEB_SERVER:$FUEGO_WEB_DIR/doc/daily
