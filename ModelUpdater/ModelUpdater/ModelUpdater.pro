#-------------------------------------------------
#
# Project created by QtCreator 2013-05-31T21:26:57
#
#-------------------------------------------------

QT += sql

QT       += core gui

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = $$qtLibraryTarget(ModelUpdater)
TEMPLATE = lib
CONFIG += shared
DESTDIR = ..
VERSION = 1.0.0

SOURCES += main.cpp\
    tLog.cpp \
    tDatabaseOp.cpp \
    tConstructModelTree.cpp \
    tClientThread.cpp \
    tClientStreamCommands.cpp \
    tClientGuiCommand.cpp \
    tCalcHash.cpp \
    socket.cpp \
    tSettings.cpp \
    MainModule.cpp \
    tModelsConveyor.cpp \
    tConveyor.cpp \
    tExportMain.cpp \
    tAutoExportMain.cpp


HEADERS  += \
    tLog.h \
    tFileStreamCommand.h \
    tDatabaseOp.h \
    tConstructModelTree.h \
    tClientThread.h \
    tClientStreamCommands.h \
    tClientGuiCommand.h \
    tCalcHash.h \
    tAbstractStreamCommand.h \
    tAbstractGuiCommand.h \
    socket.h \
    ClassFactory.h \
    tSettings.h \
    MainModule.h \
    tModelsConveyor.h \
    tConveyor.h \
    tExportMain.h \
    tAutoExportMain.h \
    tModels.h


HEADERS +=

DEFINES += MyLibrary_LIBRARY
