#-------------------------------------------------
#
# Project created by QtCreator 2016-12-14T15:55:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = hello
TEMPLATE = app


SOURCES += main.cpp\
    block.cpp \
    piece.cpp \
    chessstyle.cpp \
    board.cpp \
    gamewrapper.cpp

HEADERS  += \
    block.h \
    piece.h \
    chessstyle.h \
    board.h \
    gamewrapper.h \
    game.h

FORMS    += mainwindow.ui

RESOURCES +=
