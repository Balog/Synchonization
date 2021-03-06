#-------------------------------------------------
#
# Project created by QtCreator 2013-06-01T18:25:35
#
#-------------------------------------------------

QT       += core gui
QT += sql
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ModelsUpdater
TEMPLATE = app


SOURCES += main.cpp\
    Zast.cpp \
    tNewPath.cpp \
    tEditLoginPass.cpp \
    MainForm.cpp \
    Autoriz.cpp \
    tLog.cpp \
    tShowErrors.cpp

HEADERS  += \
    Zast.h \
    tNewPath.h \
    tEditLoginPass.h \
    MainForm.h \
    Autoriz.h \
    tLog.h \
    ../ModelUpdater/ModelUpdater/tExportMain.h \
    ../ModelUpdater/ModelUpdater/tCalcHash.h \
    tShowErrors.h


FORMS += \
    Zast.ui \
    tNewPath.ui \
    tEditLoginPass.ui \
    MainForm.ui \
    Autoriz.ui \
    tShowErrors.ui

RESOURCES += \
    Resources.qrc

OTHER_FILES +=


LIBS += -L../ModelUpdater -lModelUpdater1


INCLUDEPATH += ../ModelUpdater/ModelUpdater/MainModule.h
