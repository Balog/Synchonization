#-------------------------------------------------
#
# Project created by QtCreator 2013-06-12T12:15:20
#
#-------------------------------------------------

QT       += core gui
QT += sql
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutoLoader
TEMPLATE = app


SOURCES += main.cpp\
        tStart.cpp

HEADERS  += tStart.h \
    ../ModelUpdater/ModelUpdater/tCalcHash.h \
    ../ModelUpdater/ModelUpdater/tAutoExportMain.h

FORMS    += tStart.ui

LIBS += -L../ModelUpdater -lModelUpdater1


INCLUDEPATH += ../ModelUpdater/ModelUpdater/MainModule.h
