#-------------------------------------------------
#
# Project created by QtCreator 2015-06-28T11:09:49
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
        deviceselect.cpp \
    deviceapi.cpp \
    plistparser.cpp \
    plistserializer.cpp \
    backupworker.cpp \
    restoreworker.cpp

HEADERS  += mainwindow.h \
            deviceselect.h \
    deviceapi.h \
    plistparser.h \
    plistserializer.h \
    backupworker.h \
    restoreworker.h

FORMS    += mainwindow.ui \
            deviceselect.ui

RESOURCES += \
    resources.qrc
