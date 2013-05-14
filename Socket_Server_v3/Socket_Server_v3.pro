#-------------------------------------------------
#
# Project created by QtCreator 2013-01-21T17:05:40
#
#-------------------------------------------------
QT += sql
QT       += core gui
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Socket_Server_v3
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    server.cpp \
    tThreadClient.cpp \
    Client.cpp \
    tServerStreamCommands.cpp \
    tCalcHash.cpp \
    tReadWriteMutex.cpp \
    tFileBlocker.cpp \
    tSettings.cpp \
    tLog.cpp \
    tDatabaseOp.cpp

HEADERS  += dialog.h \
    Server.h \
    tThreadClient.h \
    Client.h \
    ClassFactory.h \
    tAbstractStreamCommand.h \
    tServerStreamCommands.h \
    tFileStreamCommand.h \
    tSettings.h \
    tCalcHash.h \
    tReadWriteMutex.h \
    tFileBlocker.h \
    tSettings.h \
    tDatabaseStreamCommand.h \
    tLog.h \
    tDatabaseOp.h

FORMS    += dialog.ui
