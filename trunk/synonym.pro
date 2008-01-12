TEMPLATE = app


INCLUDEPATH = . src src/ui src/model src/sound

HEADERS = src/model/wordgraph.h \
        src/model/worddataloader.h \
        src/model/partofspeechlistmodel.h \
        src/model/wordgraphdecorator.h \
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
        src/ui/configdialog.h \
        src/sound/pronunciationsoundfactory.h \
        src/sound/pronunciationsoundholder.h  \
        src/sound/pronunciationsoundfactoryimpl.h \
        src/sound/player.h \
        src/model/graphalgorithms.h \
        src/model/wordnetutil.h \
        src/model/relationship.h \
        src/model/pythondataloader.h
#        src/test/graphtest.h



SOURCES = src/model/wordgraph.cpp \
        src/model/worddataloader.cpp \
        src/model/wordgraphdecorator.cpp \
        src/model/partofspeechlistmodel.cpp \
        src/ui/graphnode.cpp \
        src/ui/graphedge.cpp \
        src/ui/graphscene.cpp \
        src/ui/graphcontroller.cpp \
        src/ui/graphwidget.cpp \
        src/ui/layout.cpp \
        src/ui/mainwindow.cpp  \
        src/ui/partofspeechitemdelegate.cpp \
        src/ui/wordsoundholder.cpp \
        src/ui/wordsoundimpl.cpp \
        src/ui/configdialog.cpp \
        src/sound/pronunciationsoundholder.cpp  \
        src/sound/pronunciationsoundfactoryimpl.cpp \
        src/sound/player.cpp \
        src/model/relatiohship.cpp \
        src/main.cpp \
        src/model/pythondataloader.cpp

#        src/test/graphtest.cpp 

CONFIG += warn_on \
      qt \
      debug \
      rtti \
#      release 
#qtestlib

TARGET = bin/synonym


QT += core \
xml \
network \
gui \
svg
#CONFIG -= release

DESTDIR = .

INCLUDEPATH += /usr/include/kde 
INCLUDEPATH += /usr/local/WordNet-3.0/include
INCLUDEPATH += /home/sergey/soft/WordnetTools/PythonQt-1.0/PythonQt-1.0/src
INCLUDEPATH += /usr/include/python2.5

LIBS += -L/usr/local/WordNet-3.0/lib \
-lWN

LIBS += -L$(PYTHONQT_ROOT)/lib -lPythonQt
LIBS += -L$(PYTHON_LIB) -lpython2.5
#RESOURCES += src/synonym.qrc

