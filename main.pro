TEMPLATE = app

TARGET = test

QT += widgets

CONFIG += debug
# CONFIG += release

#OBJECTS_DIR = build
#MOC_DIR = build

macx{
    CONFIG -= app_bundle
}

HEADERS += src/mainwindow.h \
           src/add.h \
           src/parse.h 

SOURCES += src/main.cpp \
           src/mainwindow.cpp \
           src/add.cpp \
           src/parse.cpp

