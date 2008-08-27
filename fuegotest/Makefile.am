bin_PROGRAMS = fuego_test

fuego_test_SOURCES = \
FuegoTestEngine.cpp \
FuegoTestMain.cpp

noinst_HEADERS = \
FuegoTestEngine.h

fuego_test_CXXFLAGS = -DUNIX=1 $(BOOST_CPPFLAGS)
fuego_test_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB)

fuego_test_LDADD = \
@top_srcdir@/gouct/libfuego_gouct.a \
@top_srcdir@/simpleplayers/libfuego_simpleplayers.a \
@top_srcdir@/go/libfuego_go.a \
@top_srcdir@/smartgame/libfuego_smartgame.a \
@top_srcdir@/gtpengine/libfuego_gtpengine.a

fuego_test_DEPENDENCIES = \
@top_srcdir@/gouct/libfuego_gouct.a \
@top_srcdir@/simpleplayers/libfuego_simpleplayers.a \
@top_srcdir@/go/libfuego_go.a \
@top_srcdir@/smartgame/libfuego_smartgame.a \
@top_srcdir@/gtpengine/libfuego_gtpengine.a

INCLUDES = -I@top_srcdir@/gtpengine -I@top_srcdir@/smartgame -I@top_srcdir@/go -I@top_srcdir@/simpleplayers -I@top_srcdir@/gouct

DISTCLEANFILES = *~
