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

INCLUDEPATH += include

SOURCES += source/main.cpp\
        source/processmonitor.cpp \
    source/appinit.cpp \
    source/monitorthread.cpp \
    source/iconhelper.cpp \
    source/tipwidget.cpp \
    source/basedlg.cpp \
    source/aboutdlg.cpp \
    source/appkey.cpp \
    source/gfxmessagebox.cpp

HEADERS  += include/processmonitor.h \
    include/appinit.h \
    include/monitorthread.h \
    include/iconhelper.h \
    include/tipwidget.h \
    include/aboutdlg.h \
    include/basedlg.h \
    include/appkey.h \
    include/gfxmessagebox.h

FORMS    += form/processmonitor.ui \
    form/aboutdlg.ui

RESOURCES += \
    res/processmonitor.qrc

RC_FILE +=  \
    res/processmonitor.rc
