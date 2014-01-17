noinst_LIBRARIES = libfuego_features.a

libfuego_features_a_SOURCES = \
FeBasicFeatures.cpp \
FeCommands.cpp \
FePattern.cpp \
FePatternBase.cpp

noinst_HEADERS = \
FeBasicFeatures.h \
FeCommands.h \
FePattern.h \
FePatternBase.h

libfuego_features_a_CPPFLAGS = \
$(BOOST_CPPFLAGS) \
-I@top_srcdir@/gtpengine \
-I@top_srcdir@/smartgame \
-I@top_srcdir@/go \
-I@top_srcdir@/gouct


DISTCLEANFILES = *~
