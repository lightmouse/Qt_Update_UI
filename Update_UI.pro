#-------------------------------------------------
#
# Project created by QtCreator 2016-08-12T22:04:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Update_UI
TEMPLATE = app

INCLUDEPATH += .\Canlib\Samples\cplusplus
INCLUDEPATH += .\Canlib\INC
LIBS += .\Canlib\Lib\MS\Canlib32.lib

SOURCES += main.cpp\
        mainwindow.cpp \
    Canlib/Samples/cplusplus/canlib_class.cpp

HEADERS  += mainwindow.h \
    Canlib/Samples/cplusplus/canlib_class.h

FORMS    += mainwindow.ui


