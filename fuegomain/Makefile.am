bin_PROGRAMS = fuego

fuego_SOURCES = \
FuegoMain.cpp \
FuegoMainEngine.cpp \
FuegoMainUtil.cpp

noinst_HEADERS = \
FuegoMainEngine.h \
FuegoMainUtil.h

fuego_CXXFLAGS = -DUNIX=1 $(BOOST_CPPFLAGS)
fuego_LDFLAGS = $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB)

fuego_LDADD = \
@top_srcdir@/gouct/libfuego_gouct.a \
@top_srcdir@/go/libfuego_go.a \
@top_srcdir@/smartgame/libfuego_smartgame.a \
@top_srcdir@/gtpengine/libfuego_gtpengine.a

fuego_DEPENDENCIES = \
@top_srcdir@/gouct/libfuego_gouct.a \
@top_srcdir@/go/libfuego_go.a \
@top_srcdir@/smartgame/libfuego_smartgame.a \
@top_srcdir@/gtpengine/libfuego_gtpengine.a

INCLUDES = -I@top_srcdir@/gtpengine -I@top_srcdir@/smartgame -I@top_srcdir@/go -I@top_srcdir@/gouct

DISTCLEANFILES = *~
