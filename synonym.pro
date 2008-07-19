TEMPLATE = app

DEFINES = QT_NO_DEBUG_OUTPUT

INCLUDEPATH = . src src/ui src/model src/audio
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
        src/audio/remoteaudiopronunciationloader.h \
        src/audio/scriptablesoundsource.h \
        src/audio/audiopronunciationloader.h \
        src/audio/localaudiopronunciationloader.h \
        src/audio/audiopronunciationloaderfactory.h \
        src/audio/soundsource.h \
        src/ui/tglayout.h \
        src/ui/forcedirectedlayout.h \
        src/ui/audioscriptpage.h

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
        src/audio/remoteaudiopronunciationloader.cpp \
        src/audio/scriptablesoundsource.cpp \
        src/audio/audiopronunciationloader.cpp \
        src/audio/localaudiopronunciationloader.cpp \
        src/audio/audiopronunciationloaderfactory.cpp \
        src/ui/tglayout.cpp \
        src/ui/forcedirectedlayout.cpp \
        src/ui/audioscriptpage.cpp


RESOURCES += src/synonym.qrc

CONFIG += warn_on \
      qt \
      rtti \
      thread \
      debug
#      release 
#qtestlib

TARGET = bin/synonym


QT += core \
xml \
network \
gui \
svg \
sql \
phonon \
script \
webkit 

DESTDIR = .

