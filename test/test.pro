#-------------------------------------------------
#
# Project created by QtCreator 2013-07-02T09:37:16
#
#-------------------------------------------------

QT       += core
QT       += network
QT       -= gui


TEMPLATE = app
TARGET = test
CONFIG   += console
CONFIG   -= app_bundle

include(../QtRedis.pri)


SOURCES += main.cpp \
    test.cpp

HEADERS += \
    test.h

