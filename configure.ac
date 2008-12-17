dnl Process this file with autoconf to produce a configure script.

AC_INIT
AC_PREREQ([2.59])
AC_CONFIG_SRCDIR([go/GoBoard.cpp])
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([fuego], [0.3])
AM_MAINTAINER_MODE
AC_PROG_CXX
AC_PROG_RANLIB
AX_BOOST_BASE([1.33.1])
AX_BOOST_THREAD
AX_BOOST_SYSTEM
AX_BOOST_FILESYSTEM
AX_BOOST_UNIT_TEST_FRAMEWORK

dnl TODO: Find a better (more portable) way to enable more optimizations by
dnl default.
AC_ARG_ENABLE([optimize],
	      AS_HELP_STRING([--enable-optimize], [set CXXFLAGS to -O3 -g (default is yes)]),
	      [optimize=$enableval],
	      [optimize=yes])
if test "x$optimize" = "xyes"
then
	CXXFLAGS="-O3 -g"
fi

AC_ARG_ENABLE([assert],
	      AS_HELP_STRING([--enable-assert], [enable assertions (default is no)]),
	      [assert=$enableval],
	      [assert=no])
if test "x$assert" = "xno"
then
	CXXFLAGS="$CXXFLAGS -DNDEBUG"
else
	CXXFLAGS="$CXXFLAGS"
fi

AX_CXXFLAGS_WARN_ALL
AX_CXXFLAGS_GCC_OPTION(-Wextra)

AC_OUTPUT([Makefile book/Makefile regression/Makefile misctests/Makefile fuegomain/Makefile fuegotest/Makefile go/Makefile gouct/Makefile gtpengine/Makefile simpleplayers/Makefile smartgame/Makefile unittestmain/Makefile])
