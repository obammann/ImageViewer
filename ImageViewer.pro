#-------------------------------------------------
#
# Project created by QtCreator 2016-05-17T14:14:05
#
#-------------------------------------------------

QT       += core gui
QT += printsupport
QT += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageViewer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imageviewer.cpp

HEADERS  += mainwindow.h \
    imageviewer.h

FORMS    += mainwindow.ui
