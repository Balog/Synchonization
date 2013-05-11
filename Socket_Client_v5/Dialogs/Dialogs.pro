#-------------------------------------------------
#
# Project created by QtCreator 2013-02-21T10:36:47
#
#-------------------------------------------------
QT += sql

QT       += core gui

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Dialogs
TEMPLATE = app


SOURCES += main.cpp\
        Zast.cpp \
    Autoriz.cpp \
    MainForm.cpp \
    tClientThread.cpp \
    tClientStreamCommands.cpp \
    tClientGuiCommand.cpp \
    tCalcHash.cpp \
    socket.cpp \
    tConveyor.cpp \
    tSettings.cpp \
    tDatabaseOp.cpp \
    tModelsConveyor.cpp \
    tLog.cpp \
    tEditLoginPass.cpp \
    tConstructModelTree.cpp

HEADERS  += Zast.h \
    Autoriz.h \
    MainForm.h \
    tFileStreamCommand.h \
    tClientThread.h \
    tClientStreamCommands.h \
    tClientGuiCommand.h \
    tCalcHash.h \
    tAbstractStreamCommand.h \
    tAbstractGuiCommand.h \
    ClassFactory.h \
    tConveyor.h \
    tSettings.h \
    socket.h \
    tDatabaseOp.h \
    tModelsConveyor.h \
    tLog.h \
    tEditLoginPass.h \
    tConstructModelTree.h

FORMS    += Zast.ui \
    Autoriz.ui \
    MainForm.ui \
    tEditLoginPass.ui
