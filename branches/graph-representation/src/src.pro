TEMPLATE = app

SOURCES += model/worddatagraph.cpp \
model/edge.cpp \
model/node.cpp \
model/worddataloader.cpp \
model/partofspeechlistmodel.cpp \
        ui/graphnode.cpp \
        ui/graphedge.cpp \
        ui/graphscene.cpp \
        ui/graphcontroller.cpp \
        ui/graphwidget.cpp \
        main.cpp \
        ui/mainwindow.cpp  \
        ui/partofspeechitemdelegate.cpp
CONFIG += warn_on \
	  thread \
          qt \
	  debug \
	  exceptions \
	  rtti
TARGET = ../bin/synonym

HEADERS += model/worddatagraph.h \
model/edge.h \
model/node.h \
model/worddataloader.h \
model/partofspeechlistmodel.h \
        ui/graphnode.h \
        ui/graphedge.h \
        ui/graphscene.h \
        ui/graphcontroller.h \
        ui/graphwidget.h \
        ui/mainwindow.h  \
        ui/partofspeechitemdelegate.h
QT += core \
xml \
network \
gui
LIBS += -L/usr/lib \
-lwordnet
