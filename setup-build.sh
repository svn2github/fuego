#!/bin/sh
# Set up some commonly used build configurations for Fuego.
# This is a convenience script for developers that assumes a recent version
# of GCC and sets up build configurations for optimized and debug builds in
# the subdirectory "build".

DEFAULT_TARGETS="dbg opt opt-9"
if test $# -eq 0 ; then
    TARGETS="$DEFAULT_TARGETS"
else
    TARGETS="$@"
fi

# Optimization options for the GCC compiler.
GCC_OPTIMIZE="-O3 -ffast-math"

aclocal
autoheader
autoreconf -i

setup() {
    TARGET="$1"
    CXX="g++"
    case "$TARGET" in
	dbg)
	    CXXFLAGS="-g -pipe"
	    CONFIGUREFLAGS="--enable-assert=yes"
	    ;;
	dbg-float)
	    CXXFLAGS="-g -pipe"
	    CONFIGUREFLAGS="--enable-assert=yes --enable-uct-value-type=float"
	    ;;
	dbg-9)
	    CXXFLAGS="-g -pipe"
	    CONFIGUREFLAGS="--enable-assert=yes --enable-max-size=9"
	    ;;
	opt)
	    CXXFLAGS="$GCC_OPTIMIZE -g -pipe"
	    CONFIGUREFLAGS=""
	    ;;
	opt-float)
	    CXXFLAGS="$GCC_OPTIMIZE -g -pipe"
	    CONFIGUREFLAGS="--enable-uct-value-type=float"
	    ;;
	opt-9)
	    CXXFLAGS="$GCC_OPTIMIZE -g -pipe"
	    CONFIGUREFLAGS="--enable-max-size=9"
	    ;;
	opt-9-float)
	    CXXFLAGS="$GCC_OPTIMIZE -g -pipe"
	    CONFIGUREFLAGS="--enable-max-size=9 --enable-uct-value-type=float"
	    ;;
	*)
	    echo "Unknown target '$TARGET'"; exit 1
	    ;;
    esac
    echo ======================================================================
    echo Setting up target $TARGET
    echo ======================================================================
    mkdir -p "build/$TARGET"
    (
	cd "build/$TARGET"
	export CXX      # Used by configure script
	export CXXFLAGS # Used by configure script
	../../configure --enable-maintainer-mode $CONFIGUREFLAGS
	make clean
    )
}

for T in $TARGETS; do setup "$T"; done
