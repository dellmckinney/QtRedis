#-------------------------------------------------
#
# Project created by QtCreator 2013-07-02T04:50:48
#
#-------------------------------------------------

QT       += network

QT       -= gui

TEMPLATE = lib
CONFIG += staticlib
TARGET = QtRedis

include(./QtRedis.pri)

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}


