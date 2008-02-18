TEMPLATE = app

DEFINES = QT_NO_DEBUG_OUTPUT

INCLUDEPATH = . src src/ui src/model
FORMS = src/ui/displayconfig.ui 
HEADERS = src/model/wordgraph.h \
        src/model/partofspeechlistmodel.h \
        src/ui/graphnode.h \
        src/ui/graphedge.h \
        src/ui/graphscene.h \
        src/ui/graphcontroller.h \
        src/ui/graphwidget.h \
        src/ui/mainwindow.h  \
        src/ui/partofspeechitemdelegate.h \
        src/ui/layout.h \
        src/ui/configdialog.h \ 
        src/model/graphalgorithms.h \
        src/model/wordnetutil.h \        
        src/model/iworddataloader.h \
        src/model/relation.h \
        src/model/dbdataloader.h \

#        src/test/graphtest.h

SOURCES = src/model/wordgraph.cpp \
        src/model/partofspeechlistmodel.cpp \
        src/ui/graphnode.cpp \
        src/ui/graphedge.cpp \
        src/ui/graphscene.cpp \
        src/ui/graphcontroller.cpp \
        src/ui/graphwidget.cpp \
        src/ui/layout.cpp \
        src/ui/mainwindow.cpp  \
        src/ui/partofspeechitemdelegate.cpp \
        src/ui/configdialog.cpp \
        src/main.cpp \
        src/model/relation.cpp \
        src/model/dbdataloader.cpp \
#        src/test/graphtest.cpp 

RESOURCES += src/synonym.qrc

CONFIG += warn_on \
      qt \
      rtti \
      thread \
      release 
#qtestlib

TARGET = bin/synonym


QT += core \
xml \
network \
gui \
svg \
sql
#CONFIG -= release

DESTDIR = .





