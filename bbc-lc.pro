#-------------------------------------------------
#
# Project created by QtCreator 2014-04-21T14:33:18
#
#-------------------------------------------------

QT       += core gui webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bbc-lc
TEMPLATE = app

SOURCES += main.cpp\
        mainview.cpp \
    commentarymodel.cpp \
    competition.cpp \
    game.cpp
HEADERS  += mainview.h \
    commentarymodel.h \
    competition.h \
    game.h \
    iobserver.h

FORMS    += mainview.ui

OTHER_FILES += \
    competitions.lst \
    getEvents.php \
    getGames.php \
    LICENSE \
    README.md \
    simple_html_dom.php \
    bbc.css

QMAKE_CXXFLAGS += -std=c++11
