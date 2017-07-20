#-------------------------------------------------
#
# Project created by QtCreator 2017-07-18T13:45:17
#
#-------------------------------------------------
DEFINES += LVIEW_VERSION=\\\"0.3.2\\\"
DEFINES += GIT_VERSION=\\\"$$system(git describe --always)\\\"

QT       += core gui
#concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

TARGET = logan
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        log.cpp \
        listboxeditableitem.cpp \
        controller.cpp \
        timerform.cpp \
    logviewer.cpp

HEADERS += \
        mainwindow.h \
        log.h \
        listboxeditableitem.h \
        controller.h \
        timerform.h \
    logviewer.h


FORMS += \
        mainwindow.ui \
        timer_setup.ui \
    logviewer.ui

RESOURCES += \
    resources.qrc
