bin_PROGRAMS = fuego_test

fuego_test_SOURCES = \
FuegoTestEngine.cpp \
FuegoTestMain.cpp

noinst_HEADERS = \
FuegoTestEngine.h

fuego_test_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB)

fuego_test_LDADD = \
../gouct/libfuego_gouct.a \
../simpleplayers/libfuego_simpleplayers.a \
../go/libfuego_go.a \
../smartgame/libfuego_smartgame.a \
../gtpengine/libfuego_gtpengine.a

fuego_test_DEPENDENCIES = \
../gouct/libfuego_gouct.a \
../simpleplayers/libfuego_simpleplayers.a \
../go/libfuego_go.a \
../smartgame/libfuego_smartgame.a \
../gtpengine/libfuego_gtpengine.a

fuego_test_CPPFLAGS = \
-DUNIX=1 $(BOOST_CPPFLAGS) \
-I@top_srcdir@/gtpengine \
-I@top_srcdir@/smartgame \
-I@top_srcdir@/go \
-I@top_srcdir@/simpleplayers \
-I@top_srcdir@/gouct

DISTCLEANFILES = *~
