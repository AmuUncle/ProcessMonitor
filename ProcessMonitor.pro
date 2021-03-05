#-------------------------------------------------
#
# Project created by QtCreator 2021-03-03T09:27:49
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProcessMonitor
TEMPLATE = app
DESTDIR     = $$PWD/bin
UI_DIR      += tmp

win32:QMAKE_LFLAGS_WINDOWS += /LARGEADDRESSAWARE

SOURCES += main.cpp\
        processmonitor.cpp \
    appinit.cpp \
    monitorthread.cpp \
    iconhelper.cpp

HEADERS  += processmonitor.h \
    appinit.h \
    monitorthread.h \
    iconhelper.h

FORMS    += processmonitor.ui

RESOURCES += \
    processmonitor.qrc

RC_FILE +=  \
    processmonitor.rc
