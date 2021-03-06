#-------------------------------------------------
#
# Project created by QtCreator 2019-05-20T16:34:02
#
#-------------------------------------------------

QT       += core gui network serialbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IOCardTool
TEMPLATE = app

CONFIG +=c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../IOCard/ \

SOURCES += main.cpp\
        mainwindow.cpp \
    multipleset.cpp \
    daqset.cpp

HEADERS  += mainwindow.h \
    multipleset.h \
    daqset.h

FORMS    += mainwindow.ui \
    multipleset.ui \
    daqset.ui

Debug:{
    DESTDIR += ../exe/debug
    LIBS += ../exe/debug/IOCard.lib
}


Release:{
    DESTDIR += ../exe/release
    LIBS += ../exe/release/IOCard.lib
}
