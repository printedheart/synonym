TEMPLATE = app


INCLUDEPATH = . src src/ui src/model src/sound

HEADERS = src/model/worddatagraph.h \
        src/model/worddataloader.h \
        src/model/partofspeechlistmodel.h \
        src/ui/graphnode.h \
        src/ui/graphedge.h \
        src/ui/graphscene.h \
        src/ui/graphcontroller.h \
        src/ui/graphwidget.h \
        src/ui/mainwindow.h  \
        src/ui/partofspeechitemdelegate.h \
        src/ui/wordsoundinterface.h \
        src/ui/wordsoundholder.h \
        src/ui/wordsoundimpl.h \
        src/ui/layout.h \
        src/sound/pronunciationsoundfactory.h \
        src/sound/pronunciationsoundholder.h  \
        src/sound/pronunciationsoundfactoryimpl.h \
        src/sound/player.h \
        src/model/graphalgorithms.h


SOURCES = src/model/worddatagraph.cpp \
        src/model/worddataloader.cpp \
        src/model/partofspeechlistmodel.cpp \
        src/ui/graphnode.cpp \
        src/ui/graphedge.cpp \
        src/ui/graphscene.cpp \
        src/ui/graphcontroller.cpp \
        src/ui/graphwidget.cpp \
        src/ui/layout.cpp \
        src/main.cpp \
        src/ui/mainwindow.cpp  \
        src/ui/partofspeechitemdelegate.cpp \
        src/ui/wordsoundholder.cpp \
        src/ui/wordsoundimpl.cpp \
        src/sound/pronunciationsoundholder.cpp  \
        src/sound/pronunciationsoundfactoryimpl.cpp \
        src/sound/player.cpp

CONFIG += warn_on \
      thread \
          qt \
      debug \
      exceptions \
      rtti
TARGET = bin/synonym


QT += core \
xml \
network \
gui \
svg
CONFIG -= release

DESTDIR = .

INCLUDEPATH += /usr/include/kde 
INCLUDEPATH += /usr/local/WordNet-3.0/include

LIBS += -L/usr/local/WordNet-3.0/lib \
-lWN
SOURCES -= src/model/edge.cpp \
src/model/node.cpp
HEADERS -= src/model/edge.h \
src/model/node.h
