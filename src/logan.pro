#-------------------------------------------------
#
# Project created by QtCreator 2017-07-18T13:45:17
#
#-------------------------------------------------
DEFINES += GIT_VERSION=\\\"$$system(git describe --always  --tags)\\\"

QT       += core gui
#concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

TARGET = logan

TEMPLATE = app

TRANSLATIONS += \
   translations/logan_ru.ts

tr.commands = lupdate $$_PRO_FILE_ && lrelease $$_PRO_FILE_
PRE_TARGETDEPS += tr
QMAKE_EXTRA_TARGETS += tr

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        log.cpp \
        listboxeditableitem.cpp \
        controller.cpp \
        timerform.cpp \
    logviewer.cpp \
    textcodecselectiondialog.cpp \
    highlighteditdialog.cpp \
    highlightmodel.cpp \
    filter.cpp \
    pattern.cpp \
    timeeditform.cpp \
    filterparamsform.cpp \
    mainwindow_connect.cpp \
    settings.cpp

HEADERS += \
        mainwindow.h \
        log.h \
        listboxeditableitem.h \
        controller.h \
        timerform.h \
    logviewer.h \
    textcodecselectiondialog.h \
    highlighteditdialog.h \
    highlightmodel.h \
    filter.h \
    common_re.h \
    pattern.h \
    timeeditform.h \
    filterparamsform.h


FORMS += \
        mainwindow.ui \
        timer_setup.ui \
    logviewer.ui \
    textcodecselectiondialog.ui \
    highlighteditdialog.ui \
    timeeditform.ui \
    filterparamsform.ui

RESOURCES += \
    resources.qrc
