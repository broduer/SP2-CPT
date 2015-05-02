# Copyright David Zhu. All rights reserved.

#-------------------------------------------------
#
# Project created by QtCreator 2015-04-19T14:38:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SP2-CPT
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mission.cpp \
    covert.cpp \
    security.cpp

HEADERS  += mainwindow.h \
    mission.h \
    covert.h \
    security.h

FORMS    += mainwindow.ui

# Qt 5.4 has an issue where CONFIG += c++14 doesn't work
QMAKE_CXXFLAGS += -std=c++14
