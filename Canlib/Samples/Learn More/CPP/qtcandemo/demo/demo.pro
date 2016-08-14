#-------------------------------------------------
#
# Project created by QtCreator 2011-11-01T20:04:02
#
#-------------------------------------------------

QT        += core gui
TARGET     = demo
TEMPLATE   = app

SOURCES   += main.cpp mainwindow.cpp canreader.cpp
HEADERS   += mainwindow.h canreader.h
FORMS     += mainwindow.ui
RESOURCES += graphics.qrc

unix {
  INCLUDEPATH += /usr/lib /usr/include
  LIBS        += -lcanlib
}

win32 {
  INCLUDEPATH += Your path here!
  LIBS        += $$PWD/canlib32.a
}
