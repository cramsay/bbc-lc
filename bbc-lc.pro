TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    competition.cpp \
    game.cpp \
    commentarymodel.cpp \
    cursesview.cpp

HEADERS += \
    competition.h \
    game.h \
    commentarymodel.h \
    iobserver.h \
    cursesview.h

QMAKE_CXXFLAGS += -std=c++11 -pthread
LIBS += -pthread -lncurses
