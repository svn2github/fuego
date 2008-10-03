bin_PROGRAMS = fuego

fuego_SOURCES = \
FuegoMain.cpp \
FuegoMainEngine.cpp \
FuegoMainUtil.cpp

noinst_HEADERS = \
FuegoMainEngine.h \
FuegoMainUtil.h

fuego_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB)

fuego_LDADD = \
../gouct/libfuego_gouct.a \
../go/libfuego_go.a \
../smartgame/libfuego_smartgame.a \
../gtpengine/libfuego_gtpengine.a

fuego_DEPENDENCIES = \
../gouct/libfuego_gouct.a \
../go/libfuego_go.a \
../smartgame/libfuego_smartgame.a \
../gtpengine/libfuego_gtpengine.a

fuego_CPPFLAGS = \
-DUNIX=1 $(BOOST_CPPFLAGS) \
-I@top_srcdir@/gtpengine \
-I@top_srcdir@/smartgame \
-I@top_srcdir@/go \
-I@top_srcdir@/gouct

DISTCLEANFILES = *~
