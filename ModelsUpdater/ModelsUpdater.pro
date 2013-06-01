#-------------------------------------------------
#
# Project created by QtCreator 2013-06-01T18:25:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ModelsUpdater
TEMPLATE = app


SOURCES += main.cpp\
    Zast.cpp \
    tNewPath.cpp \
    tEditLoginPass.cpp \
    MainForm.cpp \
    Autoriz.cpp \
    tProgress.cpp

HEADERS  += \
    Zast.h \
    tNewPath.h \
    tEditLoginPass.h \
    MainForm.h \
    Autoriz.h \
    tProgress.h

FORMS += \
    Zast.ui \
    tNewPath.ui \
    tEditLoginPass.ui \
    MainForm.ui \
    Autoriz.ui \
    tProgress.ui

RESOURCES += \
    Resources.qrc
