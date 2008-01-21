TEMPLATE = app

DEFINES = QT_NO_DEBUG_OUTPUT

INCLUDEPATH = . src src/ui src/model
FORMS = src/ui/displayconfig.ui 
HEADERS = src/model/wordgraph.h \
        src/model/worddataloader.h \
        src/model/partofspeechlistmodel.h \
        src/ui/graphnode.h \
        src/ui/graphedge.h \
        src/ui/graphscene.h \
        src/ui/graphcontroller.h \
        src/ui/graphwidget.h \
        src/ui/mainwindow.h  \
        src/ui/partofspeechitemdelegate.h \
#        src/ui/wordsoundinterface.h \
#        src/ui/wordsoundholder.h \
#        src/ui/wordsoundimpl.h \
        src/ui/layout.h \
        src/ui/configdialog.h \ 
        src/model/graphalgorithms.h \
        src/model/wordnetutil.h \        
        src/model/iworddataloader.h \
        src/model/relation.h \
#        src/model/wordgraphdecorator.h \
#        src/model/pythondataloader.h \
        
#        src/test/graphtest.h

SOURCES = src/model/wordgraph.cpp \
        src/model/worddataloader.cpp \
        src/model/partofspeechlistmodel.cpp \
        src/ui/graphnode.cpp \
        src/ui/graphedge.cpp \
        src/ui/graphscene.cpp \
        src/ui/graphcontroller.cpp \
        src/ui/graphwidget.cpp \
        src/ui/layout.cpp \
        src/ui/mainwindow.cpp  \
        src/ui/partofspeechitemdelegate.cpp \
#        src/ui/wordsoundholder.cpp \
#        src/ui/wordsoundimpl.cpp \
        src/ui/configdialog.cpp \
        src/main.cpp \
        src/model/relation.cpp \
#        src/model/wordgraphdecorator.cpp \
#        src/model/pythondataloader.cpp \
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
svg
#CONFIG -= release

DESTDIR = .

#This is only when we use python to load wordnet
#INCLUDEPATH += /home/sergey/soft/WordnetTools/PythonQt-1.0/PythonQt-1.0/src
#INCLUDEPATH += /usr/include/python2.5
#LIBS += -L$(PYTHONQT_ROOT)/lib -lPythonQt
#LIBS += -L$(PYTHON_LIB) -lpython2.5


# If you compile wordnet youself uncomment the following lines.
#INCLUDEPATH += /usr/local/WordNet-3.0/include
#LIBS += -L/usr/local/WordNet-3.0/lib -lWN

LIBS += -L/usr/lib -lwordnet




